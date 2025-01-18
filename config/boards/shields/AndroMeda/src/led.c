#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/bluetooth/services/bas.h>

#include <zmk/ble.h>
#include <zmk/endpoints.h>


#include <zmk/hid_indicators.h>
#include <zmk/events/hid_indicator_changed.h>

#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define LED_GPIO_NODE_ID DT_COMPAT_GET_ANY_STATUS_OKAY(gpio_leds)

// GPIO-based LED device and indices of red/green/blue LEDs inside its DT node
static const struct device *led_dev = DEVICE_DT_GET(LED_GPIO_NODE_ID);

static int led_capslock_listener_cb(const zmk_event_t *eh) {
    zmk_led_indicators_flags_t flags = zmk_led_indicators_get_current_flags();

    if (flags & ZMK_LED_INDICATORS_CAPSLOCK_BIT) {
        led_on(led_dev, DT_NODE_CHILD_IDX(DT_ALIAS(led_main)));

    } else {
        led_off(led_dev, DT_NODE_CHILD_IDX(DT_ALIAS(led_main)));
    }

    return 0;
}

ZMK_LISTENER(led_indicators_listener, led_capslock_listener_cb);
ZMK_SUBSCRIPTION(led_indicators_listener, zmk_led_indicators_changed);

static int leds_init(const struct device *device) {
    if (!device_is_ready(led_dev)) {
        LOG_ERR("Device %s is not ready", led_dev->name);
        return -ENODEV;
    }

    return 0;
}

// run leds_init on boot
SYS_INIT(leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);