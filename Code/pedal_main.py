import board
import busio
import adafruit_ads1x15.ads1115 as ADS
from adafruit_ads1x15.analog_in import AnalogIn
from neopixel_pio import Neopixel
import _thread
import time
import array
import network
import socket
from time import sleep
import machine
from uosc.client import Bundle, Client, create_message
import select
import sys
import ujson

from machine import Pin

settings = {
    "ssid": 'Draguve4',
    "password" : 'pioneer123',
    "OSC_Server" : '192.168.80.176',
    "OSC_Port" : 8000,
    "Verbose":False
}

with open('settings.json') as data_file:
    data = ujson.loads(data_file.read())
    for key in settings.keys():
        if key in data:
            settings[key] = data[key]
            

pixel_pin = 3  
num_pixels = 8
adc_peak = 26000

def lerp(begin,end,t):
    return begin + t*(end-begin)

def clamp(value,v_min,v_max):
    return min(max(value,v_min),v_max)
    

i2c = busio.I2C(scl=board.GP19, sda=board.GP18)

adc_values = array.array('i', [0,0,0,0])
stomp_values = array.array('i',[0,0])

def ADCThread():
    ads = ADS.ADS1115(i2c,address=73,data_rate=860)
    chan_knob0 = AnalogIn(ads, ADS.P0)
    chan_knob1 = AnalogIn(ads, ADS.P1)
    chan_knob2 = AnalogIn(ads, ADS.P2)
    chan_knob3 = AnalogIn(ads, ADS.P3)
    
    stompButton1 = Pin(11, Pin.IN, Pin.PULL_DOWN)
    stompButton2 = Pin(12, Pin.IN, Pin.PULL_DOWN)

    while(True):
        adc_values[0] = chan_knob0.value
        adc_values[1] = chan_knob1.value
        adc_values[2] = chan_knob2.value
        adc_values[3] = chan_knob3.value
        stomp_values[0] = stompButton1.value()
        stomp_values[1] = stompButton2.value()
                
_thread.start_new_thread(ADCThread, ())

pixels = Neopixel(
    pin=pixel_pin,
    num_leds=num_pixels,
    state_machine=0
)
pixels.brightness(40)

def get_wifi_status(wlan):
    return ujson.dumps({
        "ifconfig":wlan.ifconfig(),
        "is_connected":wlan.isconnected()
    })

while True:
    print("Starting Main Thread....")
    osc = None
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(settings["ssid"], settings["password"])
    last_values = array.array("f",[0,0,0,0])
    last_stomp_values = array.array('i',[0,0])

    while True:
        if wlan.isconnected() == True and osc == None:
            print(get_wifi_status(wlan))
            osc = Client(settings["OSC_Server"], settings["OSC_Port"])
        isConnected = wlan.isconnected() == True and osc != None
        for i in range(len(adc_values)):
            t_value = clamp(adc_values[i]/adc_peak,0.0,1.0)
            t_value = round(t_value,2)
            pixels.set_pixel(i,(lerp(0,255,t_value),0,0))
            if(t_value != last_values[i]):
                if isConnected:
                    osc.send(f"/controls/Pot{i}", t_value)
                if settings["Verbose"]:
                    print(f"Pot {i} {t_value}")
            last_values[i] = t_value
        for i in range(len(last_stomp_values)):
            t_value = stomp_values[i]
            pixels.set_pixel(4+i,(0,t_value*255,0))
            if(t_value != last_stomp_values[i]):
                if isConnected:
                    osc.send(f"/controls/Stomp{i}", float(t_value))
                if settings["Verbose"]:
                    print(f"Stomp {i} {t_value}")
            last_stomp_values[i] = t_value
        try:
            if select.select([sys.stdin],[],[],0)[0]:
                input_data = sys.stdin.readline()
                if input_data is not None and len(input_data)>0:
                    parsed_data = ujson.loads(input_data)
                    updated = False
                    if "getSettings" in parsed_data:
                        print(ujson.dumps(settings))
                    if "getWifiStatus" in parsed_data:
                        print(get_wifi_status(wlan))
                    for key in settings.keys():
                        if key in parsed_data:
                            settings[key] = parsed_data[key]
                            updated = True
                    if updated:
                        with open('settings.json', 'wb') as f:
                            ujson.dump(settings, f)
                        # need to restart main thread
                        break
        except Exception as e:
            if settings["Verbose"]:
                print("Could not parse JSON")
                print(e)
        pixels.show()
