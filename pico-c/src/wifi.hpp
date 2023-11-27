#ifndef WIFI_HPP
#define WIFI_HPP

#include "pico/cyw43_arch.h"
#include "config_manager.hpp"
#include "reboot.hpp"

void connect_wifi(ConfigManager *config);

#endif