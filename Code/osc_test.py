import network
import socket
from time import sleep
import machine
from uosc.client import Bundle, Client, create_message

ssid = 'Draguve4'
password = 'pioneer123'
OSC_Server = '192.168.80.176'
OSC_Port = 8000


def connect():
    #Connect to WLAN
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, password)
    while wlan.isconnected() == False:
        print('Waiting for connection...')
        sleep(1)
    print(wlan.ifconfig())
    osc = Client(OSC_Server, OSC_Port)
    while True:
        osc.send('/controls/frobnicator', 42, 3.1419, "spamm")

try:
    connect()
except KeyboardInterrupt:
    print("Could Not Connect")