import machine
import network

from time import sleep

from clock import clock
from secret import ssid, password
from redis import ping as redis_ping
from hinge_manager import HingeManager

# Connect to WLAN
wlan = network.WLAN(network.STA_IF)


AUTO_LOCK_AFTER = 20  # seconds


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


door_should_open = False
door_is_open = True
hinge_manager = HingeManager()


last_ping = clock.get_time()


def core0_thread():
    global door_should_open, door_is_open

    from redis import next_door_message

    while True:
        sleep(0.01)
        msg = ""
        try:
            msg = next_door_message()
            if msg:
                print(msg)
        except Exception as e:
            print(e)
            return
        if msg == "lock":
            door_should_open = False
        if msg == "unlock":
            door_should_open = True

        gpio_loop_iter()
        ping()


def ping():
    global last_ping
    if last_ping + 60 > clock.get_time():
        return
    last_ping = clock.get_time()
    for _ in range(3):  # 3 tries
        if redis_ping() == "pong":
            return
    raise ValueError("disconnected")


from door import lock_door, unlock_door

from machine import Pin

open_since = clock.get_time()

pin_hinge = Pin(14, mode=Pin.IN, pull=Pin.PULL_UP)
pin_handle = Pin(15, mode=Pin.IN, pull=Pin.PULL_UP)


def gpio_loop_iter():
    global door_should_open, door_is_open, open_since

    hinge_open = pin_hinge.value()
    handle_pressed = not pin_handle.value()

    if door_is_open and clock.get_time() > open_since + AUTO_LOCK_AFTER:
        door_should_open = False
    door_is_open, door_should_open, reset_timer = hinge_manager.tick(
        door_is_open, door_should_open, hinge_open
    )
    if reset_timer:
        open_since = clock.get_time()
    if handle_pressed:
        door_should_open = True

    if door_should_open == door_is_open:
        return

    door_is_open = door_should_open
    if door_is_open:
        unlock_door()
        open_since = clock.get_time()
    else:
        lock_door()


try:
    core0_thread()
except Exception:
    pass
sleep(3)
machine.reset()
