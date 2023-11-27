import machine
import network

from time import sleep_us
from clock import clock
from secret import ssid, password
from redis import ping as redis_ping
from hinge_manager import HingeManager
from door import lock_door, unlock_door
from machine import Pin
from dist import is_closer
from config_manager import config_manager

# Connect to WLAN
wlan = network.WLAN(network.STA_IF)

AUTO_LOCK_AFTER = 20  # seconds

pin_hinge = Pin(14, mode=Pin.IN, pull=Pin.PULL_UP)


hinge_manager = 0
open_since = 0
door_should_open = 0
door_is_open = 0
last_ping = 0


def reset_vars():
    global hinge_manager, open_since, door_should_open, door_is_open, last_ping

    hinge_manager = HingeManager()
    open_since = clock.get_time()
    door_should_open = False
    door_is_open = True
    last_ping = clock.get_time()


def connect():
    sleep_us(1_000_000)
    wlan.active(True)
    sleep_us(1_000_000)
    wlan.connect(ssid, password)
    fails = 0
    while wlan.isconnected() == False:
        fails += 1
        if fails > 15:
            sleep_us(1_000_000)
            wlan.active(False)
            sleep_us(1_000_000)
            return connect()
        print("Waiting for connection...")
        sleep_us(1_000_000)
    print(wlan.ifconfig()[0])


def core0_thread():
    global door_should_open, door_is_open

    from redis import next_door_message

    reset_vars()

    while True:
        sleep_us(10_000)
        msg = ""
        try:
            msg = next_door_message()
            if msg:
                last_ping = clock.get_time()
                print(msg)
        except Exception as e:
            print(e)
            return
        if msg == "lock":
            door_should_open = False
        if msg == "unlock":
            door_should_open = True
        if msg and msg.lower().startswith("set distance="):
            config_manager.set("distance", int(msg.split('=')[1].strip()))

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
        sleep_us(1_000_000)
    raise ValueError("disconnected")


def gpio_loop_iter():
    global door_should_open, door_is_open, open_since

    hinge_open = pin_hinge.value()
    closer = is_closer(config_manager.get("distance", 100))
    #print(closer)
    handle_pressed = closer

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

    last_ping = clock.get_time()

    door_is_open = door_should_open
    if door_is_open:
        unlock_door()
        open_since = clock.get_time()
    else:
        lock_door()


connect()
#core0_thread()
for _ in range(3):
    try:
        core0_thread()
    except Exception as e:
        print(e)
        pass
    sleep_us(1_000_000)

machine.reset()
