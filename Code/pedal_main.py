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

ssid = 'Draguve4'
password = 'pioneer123'
OSC_Server = '192.168.80.176'
OSC_Port = 8000

pixel_pin = 0
num_pixels = 6
adc_peak = 26000

def lerp(begin,end,t):
    return begin + t*(end-begin)

def clamp(value,v_min,v_max):
    return min(max(value,v_min),v_max)
    

i2c = busio.I2C(scl=board.GP19, sda=board.GP18)
#i2c.try_lock()
#i2c.scan()

ControlNames =[
    "/controls/Slider",
    "/controls/Knob0",
    "/controls/Knob1",
    "/controls/Knob2",
    "/controls/Knob3",
    "/controls/Knob4",
]

adc_values = array.array('i', (0 for _ in range(6)))


def ADCThread():
    ads = ADS.ADS1115(i2c,data_rate=860)
    ads2 = ADS.ADS1115(i2c,address=73,data_rate=860)

    chan_stickx = AnalogIn(ads, ADS.P0)
    chan_sticky = AnalogIn(ads, ADS.P1)
    chan_slider = AnalogIn(ads2, ADS.P0)
    chan_knob0 = AnalogIn(ads2, ADS.P1)
    chan_knob1 = AnalogIn(ads2, ADS.P2)
    chan_knob2 = AnalogIn(ads2, ADS.P3)
    chan_knob3 = AnalogIn(ads, ADS.P2)
    chan_knob4 = AnalogIn(ads, ADS.P3)


    while(True):
        adc_values[0] = chan_slider.value
        adc_values[1] = chan_knob0.value
        adc_values[2] = chan_knob1.value
        adc_values[3] = chan_knob2.value
        adc_values[4] = chan_knob3.value
        adc_values[5] = chan_knob4.value
                
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
last_values = array.array("f",(0.0 for _ in range(6)))

while True:
    if wlan.isconnected() == True and osc == None:
        print("Connected : ",wlan.ifconfig())
        osc = Client(OSC_Server, OSC_Port)
    for i in range(len(adc_values)):
        t_value = clamp(adc_values[i]/adc_peak,0.0,1.0)
        pixels.set_pixel(i,(lerp(0,255,t_value),0,0))
        t_value = round(t_value,2)
        if(t_value != last_values[i]):
            if wlan.isconnected() == True and osc != None:
                osc.send(ControlNames[i], t_value)
            print(f"{i} {t_value}")
        last_values[i] = t_value
    pixels.show()
