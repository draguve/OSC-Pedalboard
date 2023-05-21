import board
import busio
import adafruit_ads1x15.ads1115 as ADS
from adafruit_ads1x15.analog_in import AnalogIn
from neopixel_pio import Neopixel
import _thread
import time
import array

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

adc_values = array.array('i', (0 for _ in range(6)))

def LedThread():

    pixels = Neopixel(
        pin=pixel_pin,
        num_leds=num_pixels,
        state_machine=0
    )
    pixels.brightness(40)

    last_values = array.array("f",(0.0 for _ in range(6)))
    while True:
        for i in range(len(adc_values)):
            t_value = clamp(adc_values[i]/adc_peak,0.0,1.0)
            pixels.set_pixel(i,(lerp(0,255,t_value),0,0))
            t_value = round(t_value,2)
            if(t_value != last_values[i]):
                print(f"{i} {t_value}")
            last_values[i] = t_value
        pixels.show()
                
_thread.start_new_thread(LedThread, ())
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
