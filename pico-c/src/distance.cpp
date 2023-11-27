#include "distance.hpp"

// Define the pins for the HC-SR04 sensor
constexpr uint POWER_PIN = 7;
constexpr uint TRIG_PIN = 8;
constexpr uint ECHO_PIN = 9;

Distance::Distance()
{
    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_init(POWER_PIN);
    gpio_set_dir(POWER_PIN, GPIO_OUT);
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
    gpio_put(POWER_PIN, 1);
}

float Distance::measure_distance()
{
    // Send a 10us pulse to trigger the sensor
    sleep_us(2);
    gpio_put(TRIG_PIN, 0);
    sleep_us(2);
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    absolute_time_t start = get_absolute_time();
    absolute_time_t signaloff = start;
    absolute_time_t signalon = start;

    // Measure the time it takes for the pulse to come back
    while (gpio_get(ECHO_PIN) == 0 && absolute_time_diff_us(start, get_absolute_time()) < 9000)
    {
        signaloff = get_absolute_time();
    }
    while (gpio_get(ECHO_PIN) == 1 && absolute_time_diff_us(start, get_absolute_time()) < 9000)
    {
        signalon = get_absolute_time();
    }

    // Calculate the duration of the pulse
    uint64_t timepassed = absolute_time_diff_us(signaloff, signalon);

    // Calculate the distance in centimeters
    float distance = (timepassed * 0.0343) / 2;

    return distance > 150 ? 150 : distance;
}

bool Distance::is_closer_single(int cm)
{
    float measured = measure_distance();
    if (cm < 0)
        return measured >= -cm;

    return measured <= cm;
}

bool Distance::is_closer(int cm)
{
    for (int i = 0; i < 3; ++i)
        if (!is_closer_single(cm))
            return false;

    return true;
}
