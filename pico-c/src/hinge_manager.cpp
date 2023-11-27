#include "hinge_manager.hpp"

HingeManager::HingeManager() : pin_open(false), was_open_for_some_time(false)
{
    pin_open_since = get_absolute_time();
    pin_closed_since = get_absolute_time();
}

bool HingeManager::is_open_for_some_time() const
{
    return pin_open && absolute_time_diff_us(pin_open_since, get_absolute_time()) > OPEN_DELTA * 1e6;
}

bool HingeManager::is_closed_for_some_time() const
{
    return !pin_open && absolute_time_diff_us(pin_closed_since, get_absolute_time()) > CLOSED_DELTA * 1e6;
}

std::tuple<bool, bool, bool> HingeManager::tick(bool door_is, bool door_should_be, bool hinge_open)
{
    bool reset_timer = false;
    bool was_open = pin_open;
    pin_open = hinge_open;

    if (was_open != hinge_open)
    {
        if (hinge_open)
        {
            pin_open_since = get_absolute_time();
        }
        else
        {
            pin_closed_since = get_absolute_time();
        }
    }

    if (is_open_for_some_time())
    {
        was_open_for_some_time = true;
        reset_timer = true;
    }

    if (is_closed_for_some_time() && was_open_for_some_time)
    {
        was_open_for_some_time = false;
        door_is = true;
        reset_timer = true;
    }

    return {door_is, door_should_be, reset_timer};
}
