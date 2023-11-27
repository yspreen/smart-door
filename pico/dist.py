import machine
import time

# Define the pins for the HC-SR04 sensor
trig_pin = 8  # Replace with the actual GPIO pin you're using for trigger
echo_pin = 9  # Replace with the actual GPIO pin you're using for echo
power_pin = 7

# Set up the trigger and echo pins
trigger = machine.Pin(trig_pin, machine.Pin.OUT)
machine.Pin(power_pin, machine.Pin.OUT).value(1)

def measure_distance():
    echo = machine.Pin(echo_pin, machine.Pin.IN)
    # Send a 10us pulse to trigger the sensor
    trigger.low()
    time.sleep_us(2)  # Wait for 2 microseconds
    trigger.high()
    time.sleep_us(10)  # Send a 10-microsecond pulse
    trigger.low()
    
    signalon = signaloff = start = time.ticks_us()

    # Measure the time it takes for the pulse to come back
    while echo.value() == 0 and time.ticks_us() - start < 20_000:
        signaloff = time.ticks_us()
    while echo.value() == 1 and time.ticks_us() - start < 20_000:
        signalon = time.ticks_us()

    # Calculate the duration of the pulse
    timepassed = signalon - signaloff

    # Calculate the distance in centimeters
    # Speed of sound is approximately 340 m/s,
    # so it travels 1cm in about 29 microseconds
    # We divide by 2 because there and back
    distance = (timepassed * 0.0343) / 2
    
    machine.Pin(echo_pin, machine.Pin.OUT).value(0)

    #machine.Pin(power_pin, machine.Pin.OUT).value(0)
    return 250 if distance > 250 else distance

## is_closer(5) is true if the value is <=5cm
## is_closer(-5) is true if the value is >=5cm
def is_closer_single(cm):
    measured = measure_distance()
    #print(measured)
    if cm < 0:
        return measured >= -cm
    return measured <= cm

## is_closer(5) is true if the value is <=5cm
## is_closer(-5) is true if the value is >=5cm
def is_closer(cm):
    for _ in range(3):
        if not is_closer_single(cm):
            return False
    return True
