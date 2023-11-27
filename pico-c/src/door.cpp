#include "door.hpp"

#include <limits>

// Define GPIO pins
constexpr uint SENSOR_POWER_PIN = 7;
constexpr uint RELAY_PIN = 18;
constexpr uint SERVO_PIN = 28; // PWM pin for the servo

// Servo and relay timings
constexpr uint16_t UNLOCKED_ANGLE = uint16_t(1000 + 8000 * 0 / 180); // 0 degrees
constexpr uint16_t LOCKED_ANGLE = uint16_t(1000 + 8000 * 60 / 180);  // 60 degrees
constexpr uint RELAY_DELAY_US = 100000;
constexpr uint SERVO_DELAY_US = 2000000;

constexpr uint16_t max_int = std::numeric_limits<uint16_t>::max();
constexpr uint16_t wrap = 20000U;

void set_relay_on(bool v)
{
    if (v)
    {
        gpio_put(SENSOR_POWER_PIN, 0);
        sleep_us(10000);
        gpio_put(RELAY_PIN, 1);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    }
    else
    {
        gpio_put(RELAY_PIN, 0);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_us(10000);
        gpio_put(SENSOR_POWER_PIN, 1);
    }
}

void move_servo_to(uint16_t angle)
{
    double percentage = double(angle) / double(max_int);
    pwm_set_gpio_level(SERVO_PIN, uint16_t(double(wrap) * percentage));
    sleep_us(10000);
}

void setup_pwm()
{
    // clock speed is 125MHz
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_clkdiv(slice_num, 125.0f); // Adjusted for 50Hz at 125MHz system clock
    pwm_set_enabled(slice_num, true);
}

void Door::unlock_door()
{
    std::cout << ("Unlocking door\n");
    set_relay_on(true);
    sleep_us(RELAY_DELAY_US);
    move_servo_to(UNLOCKED_ANGLE);
    sleep_us(SERVO_DELAY_US);
    set_relay_on(false);
    sleep_us(RELAY_DELAY_US);
}

void Door::lock_door()
{
    std::cout << ("Locking door\n");
    set_relay_on(true);
    sleep_us(RELAY_DELAY_US);
    move_servo_to(LOCKED_ANGLE);
    sleep_us(SERVO_DELAY_US);
    set_relay_on(false);
    sleep_us(RELAY_DELAY_US);
}

Door::Door()
{
    // Initialize GPIO
    gpio_init(RELAY_PIN);
    gpio_set_dir(RELAY_PIN, GPIO_OUT);

    // Initialize PWM
    setup_pwm();
}
