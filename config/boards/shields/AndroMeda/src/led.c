#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/events/hid_indicators_changed.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define LED_GPIO_NODE_ID DT_COMPAT_GET_ANY_STATUS_OKAY(gpio_leds)

BUILD_ASSERT(DT_NODE_EXISTS(DT_ALIAS(indicator_led)),
             "An alias for the indicator LED is not found for LED_INDICATOR");

static const struct device *led_dev = DEVICE_DT_GET(LED_GPIO_NODE_ID);
static const uint8_t led_idx = DT_NODE_CHILD_IDX(DT_ALIAS(indicator_led));

#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)

static int led_capslock_listener_cb(const zmk_event_t *eh) {
    zmk_led_indicators_flags_t flags = zmk_led_indicators_get_current_flags();
    
    if (flags & ZMK_LED_INDICATORS_CAPSLOCK_BIT) {
        // Turn the LED on when Caps Lock is active
        led_on(led_dev, led_idx);
    } else {
        // Turn the LED off when Caps Lock is inactive
        led_off(led_dev, led_idx);
    }
    return 0;
}

ZMK_LISTENER(led_indicators_listener, led_capslock_listener_cb);
ZMK_SUBSCRIPTION(led_indicators_listener, zmk_led_indicators_changed);

#endif // IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)
