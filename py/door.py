from time import sleep
from machine import Pin, PWM

pwm = PWM(Pin(28))
pwm.freq(50)

locked_angle = 1000
unlocked_angle = 9000
relay_delay_s = 0.1
servo_delay_s = 3.0


def set_relay_on(v):
    Pin(18, mode=Pin.OUT).value(1 if v else 0)
    Pin("LED").value(1 if v else 0)


def move_servo_to(angle):
    pwm.duty_u16(angle)
    sleep(0.01)


def unlock_door():
    set_relay_on(True)
    sleep(relay_delay_s)
    move_servo_to(unlocked_angle)
    sleep(servo_delay_s)
    set_relay_on(False)
    sleep(relay_delay_s)


def lock_door():
    set_relay_on(True)
    sleep(relay_delay_s)
    move_servo_to(locked_angle)
    sleep(servo_delay_s)
    set_relay_on(False)
    sleep(relay_delay_s)