#include "distance.hpp"

// Define the I2C pins for the Raspberry Pi Pico
constexpr uint I2C_SDA = 8;
constexpr uint I2C_SCL = 9;
constexpr uint I2C_VIN = 7;

Distance::Distance()
{
    gpio_init(I2C_VIN);
    gpio_set_dir(I2C_VIN, GPIO_OUT);
    gpio_put(I2C_VIN, 1);

    i2c_init(i2c0, 400 * 1000); // Initialize I2C0 at 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Initialize VL53L0X sensor
    sensor.init();
    sensor.setTimeout(500);

    // Start continuous back-to-back mode (take readings as fast as possible)
    sensor.startContinuous();
}

float Distance::measure_distance()
{
    return sensor.readRangeContinuousMillimeters() / 10;
}

bool Distance::is_closer(int cm)
{
    float measured = measure_distance();
    if (cm < 0)
        return measured >= -cm;

    return measured <= cm;
}
