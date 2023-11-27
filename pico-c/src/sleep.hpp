#ifndef SLEEP_HPP
#define SLEEP_HPP

#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "tusb.h"

#include <iostream>

void sleepMs(uint32_t timeout_ms);

#endif
