#include "sleep.hpp"

void bootsel()
{
    rom_reset_usb_boot_fn func = (rom_reset_usb_boot_fn)rom_func_lookup(ROM_FUNC_RESET_USB_BOOT);
    func(0, 0);
}

void sleepMs(uint32_t timeout_ms)
{
    auto start_time = get_absolute_time();
    while (absolute_time_diff_us(start_time, get_absolute_time()) < timeout_ms * 1000)
    {
        // Check if data is available
        if (tud_cdc_connected() && tud_cdc_available())
        {
            char ch = getchar(); // Read a character
            if (ch == 'x')
            {
                std::cout << "Rebooting to BOOTSEL..." << std::endl;
                sleep_ms(1000);
                bootsel();
                sleep_ms(1000);
            }
        }
        sleep_ms(std::min(timeout_ms, uint32_t(10)));
    }
}
