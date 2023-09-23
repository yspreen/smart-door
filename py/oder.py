from time import sleep

import machine


import network
from secret import ssid, password

# Connect to WLAN
wlan = network.WLAN(network.STA_IF)


def connect():
    sleep(1)
    wlan.active(True)
    sleep(1)
    wlan.connect(ssid, password)
    fails = 0
    while wlan.isconnected() == False:
        fails += 1
        if fails > 15:
            sleep(1)
            wlan.active(False)
            sleep(1)
            return connect()
        print("Waiting for connection...")
        sleep(1)
    print(wlan.ifconfig()[0])


try:
    connect()
except KeyboardInterrupt:
    machine.reset()


door_should_be = False
door_is = False


def core0_thread():
    global door_should_be, door_is

    from redis import next_door_message

    while True:
        sleep(0.05)
        msg = ""
        try:
            msg = next_door_message()
        except Exception as e:
            print(e)
        if msg == "lock":
            door_should_be = False
        if msg == "unlock":
            door_should_be = True

        gpio_loop_iter()


from door import lock_door, unlock_door


def gpio_loop_iter():
    global door_should_be, door_is

    if door_should_be == door_is:
        return
    door_is = door_should_be
    if door_is:
        unlock_door()
    else:
        lock_door()


core0_thread()
machine.reset()
