#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/events/hid_indicators_changed.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define LED_GPIO_NODE_ID DT_COMPAT_GET_ANY_STATUS_OKAY(gpio_leds)

BUILD_ASSERT(DT_NODE_EXISTS(DT_ALIAS(indicator_led)),
             "An alias for the Caps Lock LED is not found for LED_INDICATOR");

static const struct device *led_dev = DEVICE_DT_GET(LED_GPIO_NODE_ID);
static const uint8_t led_idx = DT_NODE_CHILD_IDX(DT_ALIAS(indicator_led));

static bool initialized = false;

#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)
static int caps_lock_listener_cb(const zmk_event_t *eh) {
    if (!initialized) {
        return 0;
    }

    zmk_hid_indicators_t indicators = as_zmk_hid_indicators_changed(eh)->indicators;

    if (indicators & HID_USAGE_LED_CAPS_LOCK) {
        LOG_INF("Caps Lock ON: Turning LED on");
        led_on(led_dev, led_idx);
    } else {
        LOG_INF("Caps Lock OFF: Turning LED off");
        led_off(led_dev, led_idx);
    }

    return 0;
}

ZMK_LISTENER(caps_lock_listener, caps_lock_listener_cb);
ZMK_SUBSCRIPTION(caps_lock_listener, zmk_hid_indicators_changed);
#endif // IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)

static int led_init(const struct device *dev) {
    ARG_UNUSED(dev);

    if (!device_is_ready(led_dev)) {
        LOG_ERR("LED device not ready");
        return -ENODEV;
    }

    LOG_INF("Caps Lock LED initialized");
    initialized = true;
    return 0;
}

SYS_INIT(led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
