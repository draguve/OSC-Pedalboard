import board
import busio
import adafruit_ads1x15.ads1115 as ADS
from adafruit_ads1x15.analog_in import AnalogIn

i2c = busio.I2C(scl=board.GP19, sda=board.GP18)
#i2c.try_lock()
#i2c.scan()

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
    #print("Slider",chan_slider.value, chan_slider.voltage)
    #print("Knob_Pedal",chan_knob0.value,chan_knob0.voltage)
    #print("Knob1",chan_knob1.value,chan_knob1.voltage)
    #print("Knob2",chan_knob2.value,chan_knob2.voltage)
    #print("Knob3",chan_knob3.value,chan_knob3.voltage)
    #print("Knob4",chan_knob4.value,chan_knob4.voltage)
    #print("StickX",chan_stickx.value, chan_stickx.voltage)
    #print("StickY",chan_sticky.value, chan_sticky.voltage)
