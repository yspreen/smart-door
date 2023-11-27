#include "lwipopts.h"

#include <pico/stdlib.h>
#include <sleep.hpp>
#include <tcp.hpp>
#include <wifi.hpp>
#include <config_manager.hpp>
#include <door_state.hpp>
#include <set_secrets.hpp>

int loop()
{
    while (true)
    {
        std::cout << "stdout" << std::endl;
        sleepMs(10000);
    }
    return 0;
}

int main()
{
    stdio_init_all();
    sleepMs(5 * 1000);

    auto config = new ConfigManager();
    set_secrets(config);
    connect_wifi(config);

    auto client = new TcpClient(config, config->get_str("redis_domain"), config->get_int("redis_port"));
    client->connect();
    auto door = new DoorState(config);

    while (true)
    {
        auto msg_ = client->next_door_message();
        if (msg_.has_value())
        {
            std::string msg = msg_.value();
            std::cout << "." << msg << "." << std::endl;
            if (msg == std::string("unlock"))
            {
                door->open_from_phone();
            }
        }
        door->loop();
        sleepMs(50);
    }

    delete client;
    cyw43_arch_deinit();
    return loop();
}