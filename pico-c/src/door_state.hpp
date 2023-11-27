#ifndef DOOR_STATE_HPP
#define DOOR_STATE_HPP

#include <pico/stdlib.h>
#include "hardware/gpio.h"
#include "hinge_manager.hpp"
#include "config_manager.hpp"
#include "distance.hpp"
#include "door.hpp"

class DoorState
{
private:
    Door door;
    HingeManager hinge;
    Distance distance;
    absolute_time_t open_since;
    bool door_is_open = false;
    bool door_should_open = false;
    bool hinge_open = false;
    ConfigManager *config;

public:
    DoorState(ConfigManager *config);

    void open_from_phone();
    void loop();
};
#endif
