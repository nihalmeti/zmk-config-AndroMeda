#include <zmk/events/hid_indicators_changed.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Define the device for your LED
static const struct device *led_dev = DEVICE_DT_GET(DT_ALIAS(indicator_led));

// Callback for handling HID indicator changes
static int hid_capslock_listener_cb(const zmk_event_t *eh) {
    const struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);
    if (ev == NULL) {
        return -ENOTSUP;
    }

    if (ev->indicators & HID_USAGE_LED_CAPS_LOCK) {
        // Turn on the LED
        if (led_dev != NULL && device_is_ready(led_dev)) {
            gpio_pin_set(led_dev, DT_GPIO_PIN(DT_ALIAS(indicator_led), gpios), 1);
        } else {
            LOG_ERR("LED device not ready");
        }
    } else {
        // Turn off the LED
        if (led_dev != NULL && device_is_ready(led_dev)) {
            gpio_pin_set(led_dev, DT_GPIO_PIN(DT_ALIAS(indicator_led), gpios), 0);
        } else {
            LOG_ERR("LED device not ready");
        }
    }
    return 0;
}

// Register the listener
ZMK_LISTENER(hid_indicators_listener, hid_capslock_listener_cb);
ZMK_SUBSCRIPTION(hid_indicators_listener, zmk_hid_indicators_changed);
