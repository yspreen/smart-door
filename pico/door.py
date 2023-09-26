from time import sleep
from machine import Pin, PWM

from redis import send_locked

pwm = PWM(Pin(28))
pwm.freq(50)


## measured
# 315 - 257 = 58

unlocked_angle = 1000 + 8000 * 0 / 180  # 0deg
locked_angle = 1000 + 8000 * 60 / 180  # 60deg
relay_delay_s = 0.1
servo_delay_s = 2.0


def set_relay_on(v):
    Pin(18, mode=Pin.OUT).value(1 if v else 0)
    Pin("LED").value(1 if v else 0)


def move_servo_to(angle):
    pwm.duty_u16(int(angle))
    sleep(0.01)


def unlock_door():
    send_locked(False)
    set_relay_on(True)
    sleep(relay_delay_s)
    move_servo_to(unlocked_angle)
    sleep(servo_delay_s)
    set_relay_on(False)
    sleep(relay_delay_s)


def lock_door():
    send_locked(True)
    set_relay_on(True)
    sleep(relay_delay_s)
    move_servo_to(locked_angle)
    sleep(servo_delay_s)
    set_relay_on(False)
    sleep(relay_delay_s)
