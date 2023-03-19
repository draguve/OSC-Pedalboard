import board
import busio
import adafruit_ads1x15.ads1115 as ADS
from adafruit_ads1x15.analog_in import AnalogIn

i2c = busio.I2C(scl=board.GP19, sda=board.GP18)
#i2c.try_lock()
#i2c.scan()

#ads = ADS.ADS1115(i2c)
ads2 = ADS.ADS1115(i2c,address=73)

chan = AnalogIn(ads2, ADS.P0)
while(True):
    print(chan.value, chan.voltage)
