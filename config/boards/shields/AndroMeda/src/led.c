#include <zmk/events/split_peripheral_status_changed.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/led_indicators.h>
#include <zmk/events/led_indicator_changed.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Define the device binding for led_0
static const struct device *led_dev;

static int led_capslock_listener_cb(const zmk_event_t *eh) {
    zmk_led_indicators_flags_t flags = zmk_led_indicators_get_current_flags();

    if (flags & ZMK_LED_INDICATORS_CAPSLOCK_BIT) {
        // Turn on the LED
        gpio_pin_set(led_dev, 2, 1);
    } else {
        // Turn off the LED
        gpio_pin_set(led_dev, 2, 0);
    }

    return 0;
}

ZMK_LISTENER(led_indicators_listener, led_capslock_listener_cb);
ZMK_SUBSCRIPTION(led_indicators_listener, zmk_led_indicators_changed);

extern void led_thread(void *d0, void *d1, void *d2) {
    ARG_UNUSED(d0);
    ARG_UNUSED(d1);
    ARG_UNUSED(d2);

    // Get the device binding for led_0
    led_dev = device_get_binding("led_0");
    if (!led_dev) {
        LOG_ERR("Failed to bind to led_0 device");
        return;
    }

    // Configure the GPIO pin
    int ret = gpio_pin_configure(led_dev, 2, GPIO_OUTPUT_ACTIVE | GPIO_ACTIVE_HIGH);
    if (ret < 0) {
        LOG_ERR("Failed to configure led_0 GPIO: %d", ret);
        return;
    }
}
