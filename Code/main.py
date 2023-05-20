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
pixels = Neopixel(
    pin=pixel_pin,
    num_leds=num_pixels,
    state_machine=0
)
pixels.brightness(40)

def clerp(begin,end,t):
    return min(max(begin + t*(end-begin), begin), end)

RED = (255, 0, 0)
YELLOW = (255, 150, 0)
GREEN = (0, 255, 0)
CYAN = (0, 255, 255)
BLUE = (0, 0, 255)
PURPLE = (180, 0, 255)

i2c = busio.I2C(scl=board.GP19, sda=board.GP18)
#i2c.try_lock()
#i2c.scan()

adc_values = array.array('i', (0 for _ in range(6)))

def LedThread():
    while True:
        pixels.set_pixel(0,(clerp(0,255,adc_values[0]/32767),0,0))
        pixels.set_pixel(1,(clerp(0,255,adc_values[1]/32767),0,0))
        pixels.set_pixel(2,(clerp(0,255,adc_values[2]/32767),0,0))
        pixels.set_pixel(3,(clerp(0,255,adc_values[3]/32767),0,0))
        pixels.set_pixel(4,(clerp(0,255,adc_values[4]/32767),0,0))
        pixels.set_pixel(5,(clerp(0,255,adc_values[5]/32767),0,0))
        pixels.show()
        
_thread.start_new_thread(LedThread, ())
ads = ADS.ADS1115(i2c)
ads2 = ADS.ADS1115(i2c,address=73)

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
