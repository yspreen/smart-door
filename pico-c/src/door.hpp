#ifndef DOOR_HPP
#define DOOR_HPP

#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <iostream>

class Door
{
public:
    Door();
    void unlock_door();
    void lock_door();
};

#endif // DOOR_HPP
