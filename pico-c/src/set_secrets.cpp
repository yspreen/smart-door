#include "set_secrets.hpp"

void set_secrets(ConfigManager *config)
{
    config->set_str("wifi_ssid", "***REMOVED***");
    config->set_str("wifi_pw", "***REMOVED***");
    config->set_str("redis_domain", "***REMOVED***");
    config->set_int("redis_port", ***REMOVED***);
    config->set_str("redis_user", "***REMOVED***");
    config->set_str("redis_pw", "***REMOVED***");
    config->set_str("redis_door", "***REMOVED***");
}
