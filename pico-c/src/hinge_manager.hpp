#ifndef HINGE_MANAGER_HPP
#define HINGE_MANAGER_HPP

#include <tuple>
#include "pico/time.h"

class HingeManager
{
public:
    HingeManager();

    bool is_open_for_some_time() const;
    bool is_closed_for_some_time() const;
    std::tuple<bool, bool, bool> tick(bool door_is, bool door_should_be, bool hinge_open);

private:
    bool pin_open;
    absolute_time_t pin_open_since;
    absolute_time_t pin_closed_since;
    bool was_open_for_some_time;

    static constexpr double OPEN_DELTA = 2.0;   // seconds
    static constexpr double CLOSED_DELTA = 2.0; // seconds
};

#endif // HINGE_MANAGER_HPP
