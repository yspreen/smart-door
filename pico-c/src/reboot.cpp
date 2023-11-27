#include "reboot.hpp"

void reboot()
{
    watchdog_reboot(0, SRAM_END, 10);
    sleep_ms(1000);
}
