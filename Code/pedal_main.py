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

from machine import Pin

ssid = 'Draguve4'
password = 'pioneer123'
OSC_Server = '192.168.80.176'
OSC_Port = 8000

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

osc = None
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect(ssid, password)
print("Here")
last_values = array.array("f",[0,0,0,0])
last_stomp_values = array.array('i',[0,0])

while True:
    if wlan.isconnected() == True and osc == None:
        print("Connected : ",wlan.ifconfig())
        osc = Client(OSC_Server, OSC_Port)
    isConnected = wlan.isconnected() == True and osc != None
    for i in range(len(adc_values)):
        t_value = clamp(adc_values[i]/adc_peak,0.0,1.0)
        t_value = round(t_value,2)
        pixels.set_pixel(i,(lerp(0,255,t_value),0,0))
        if(t_value != last_values[i]):
            if isConnected:
                osc.send(f"/controls/Pot{i}", t_value)
            print(f"Pot {i} {t_value}")
        last_values[i] = t_value
    for i in range(len(last_stomp_values)):
        t_value = stomp_values[i]
        pixels.set_pixel(4+i,(0,t_value*255,0))
        if(t_value != last_stomp_values[i]):
            if isConnected:
                osc.send(f"/controls/Stomp{i}", float(t_value))
            print(f"Stomp {i} {t_value}")
        last_stomp_values[i] = t_value
    pixels.show()
