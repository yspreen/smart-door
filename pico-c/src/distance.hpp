#ifndef DISTANCE_HPP
#define DISTANCE_HPP

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "VL53L0X.h"

class Distance
{
public:
    Distance();
    bool is_closer(int cm);

private:
    float measure_distance();
    VL53L0X sensor;
};

#endif // DISTANCE_HPP
