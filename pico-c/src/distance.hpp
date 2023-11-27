#ifndef DISTANCE_HPP
#define DISTANCE_HPP

#include "pico/stdlib.h"
#include "hardware/gpio.h"

class Distance
{
public:
    Distance();
    bool is_closer(int cm);

private:
    float measure_distance();
    bool is_closer_single(int cm);
};

#endif // DISTANCE_HPP
