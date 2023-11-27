from time import sleep_us
from machine import Pin, PWM

from redis import send_locked

pwm = PWM(Pin(28))
pwm.freq(50)


## measured
# 315 - 257 = 58

unlocked_angle = 1000 + 8000 * 0 / 180  # 0deg
locked_angle = 1000 + 8000 * 60 / 180  # 60deg
relay_delay_us =   100_000
servo_delay_us = 2_000_000

def set_relay_on(v):
    Pin(7, mode=Pin.OUT).value(0 if v else 1)
    Pin(18, mode=Pin.OUT).value(1 if v else 0)
    Pin("LED").value(1 if v else 0)


def move_servo_to(angle):
    pwm.duty_u16(int(angle))
    sleep_us(10_000)


def unlock_door():
    print("unlocking door")
    send_locked(False)
    set_relay_on(True)
    sleep_us(relay_delay_us)
    move_servo_to(unlocked_angle)
    sleep_us(servo_delay_us)
    set_relay_on(False)
    sleep_us(relay_delay_us)


def lock_door():
    print("locking door")
    send_locked(True)
    set_relay_on(True)
    sleep_us(relay_delay_us)
    move_servo_to(locked_angle)
    sleep_us(servo_delay_us)
    set_relay_on(False)
    sleep_us(relay_delay_us)
