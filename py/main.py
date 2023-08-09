import network
import socket
from time import sleep

# from picozero import pico_temp_sensor, pico_led
import machine

ssid = 'DONTCOMMIT'
password = 'DONTCOMMIT'


def connect():
    # Connect to WLAN
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, password)
    while wlan.isconnected() == False:
        print("Waiting for connection...")
        sleep(1)
    print(wlan.ifconfig()[0])


try:
    connect()
except KeyboardInterrupt:
    machine.reset()

from redis import next_door_message

while True:
    msg = next_door_message()
    if msg == "lock":
        machine.Pin("LED", machine.Pin.OUT).value(0)
    if msg == "unlock":
        machine.Pin("LED", machine.Pin.OUT).value(1)
