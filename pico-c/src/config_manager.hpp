#ifndef CONFIGMANAGER_HPP
#define CONFIGMANAGER_HPP

#include <string>
#include <cstring>
#include <sstream>
#include "json.hpp"
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

#ifndef XIP_BASE
#define XIP_BASE 0x10000000
#endif /* XIP_BASE */
#ifndef FLASH_PAGE_SIZE
#define FLASH_PAGE_SIZE 4096
#endif /* FLASH_PAGE_SIZE */

extern char __flash_binary_end;

class ConfigManager
{
public:
    ConfigManager();

    int get_int(const std::string &key, int defaultValue = 0);
    void set_int(const std::string &key, int value);

    std::string get_str(const std::string &key, const std::string &defaultValue = "");
    void set_str(const std::string &key, const std::string &value);

    void load();
    void save();

private:
    nlohmann::json config;
    void readFlash();
    void writeFlash();
    uint targetOffset;
};

#endif // CONFIGMANAGER_HPP
