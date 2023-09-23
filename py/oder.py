import network
import socket
from time import sleep
from time import sleep

# from picozero import pico_temp_sensor, pico_led
import machine

from redis import next_door_message
from door import lock_door, unlock_door
from secret import *


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


import _thread

door_should_be = False
door_is = False


def core0_thread():
    global door_should_be, door_is
    while True:
        msg = ""
        try:
            msg = next_door_message()
        except:
            pass
        if msg == "lock":
            door_should_be = False
        if msg == "unlock":
            door_should_be = True


def core1_thread():
    global door_should_be, door_is
    while True:
        sleep(0.05)
        if door_should_be == door_is:
            continue
        door_is = door_should_be
        if door_is:
            unlock_door()
        else:
            lock_door()


second_thread = _thread.start_new_thread(core1_thread, ())

core0_thread()
