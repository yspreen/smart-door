#include <door_state.hpp>

constexpr uint AUTO_LOCK_AFTER = 60; // Assuming 60 seconds for auto lock
constexpr uint PIN_HINGE = 14;       // Hinge GPIO pin number

void DoorState::open_from_phone()
{
    door_should_open = true;
}

DoorState::DoorState(ConfigManager *config)
{
    this->config = config;
    open_since = get_absolute_time();
    hinge = HingeManager();
    distance = Distance();
    door = Door();

    gpio_init(PIN_HINGE);
    gpio_set_dir(PIN_HINGE, GPIO_IN);
    gpio_pull_up(PIN_HINGE);
}

void DoorState::loop()
{
    bool hinge_open = gpio_get(PIN_HINGE);
    bool closer = distance.is_closer(config->get_int("distance", 100));
    bool handle_pressed = closer;

    if (door_is_open && absolute_time_diff_us(open_since, get_absolute_time()) > AUTO_LOCK_AFTER * 1e6)
        door_should_open = false;

    bool reset_timer;
    std::tie(door_is_open, door_should_open, reset_timer) = hinge.tick(door_is_open, door_should_open, hinge_open);

    if (reset_timer)
        open_since = get_absolute_time();

    if (handle_pressed)
        door_should_open = true;

    if (door_should_open == door_is_open)
        return;

    door_is_open = door_should_open;
    if (door_is_open)
    {
        door.unlock_door();
        open_since = get_absolute_time();
    }
    else
        door.lock_door();
}
