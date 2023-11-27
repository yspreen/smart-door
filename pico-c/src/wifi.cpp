#include "wifi.hpp"

void connect_wifi(ConfigManager *config)
{
    if (cyw43_arch_init())
    {
        printf("failed to initialise\n");
        reboot();
    }
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    auto ssid = config->get_str("wifi_ssid", "ssid");
    auto pw = config->get_str("wifi_pw", "pw");
    printf(ssid.c_str());
    printf("\n");
    printf(pw.c_str());
    printf("\n");
    if (cyw43_arch_wifi_connect_timeout_ms(ssid.c_str(), pw.c_str(), CYW43_AUTH_WPA2_AES_PSK, 30000))
    {
        printf("failed to connect.\n");
        reboot();
    }
    else
    {
        printf("Connected.\n");
    }
}
