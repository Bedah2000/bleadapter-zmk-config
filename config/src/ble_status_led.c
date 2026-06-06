/*
 * BLE status LED for BLEAdapter8.
 *
 * Blue blinking: active BLE profile is open/pairable.
 * Cyan blinking: active BLE profile is bonded but currently disconnected.
 * Green steady: active BLE profile is connected.
 */

#include <device.h>
#include <drivers/led_strip.h>
#include <errno.h>
#include <init.h>
#include <kernel.h>
#include <logging/log.h>
#include <stdbool.h>
#include <stdint.h>

#include <zmk/ble.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define STRIP_LABEL DT_LABEL(DT_CHOSEN(zmk_underglow))
#define STRIP_NUM_PIXELS DT_PROP(DT_CHOSEN(zmk_underglow), chain_length)

enum ble_status_led_state {
    BLE_STATUS_LED_UNKNOWN,
    BLE_STATUS_LED_PAIRING,
    BLE_STATUS_LED_RECONNECTING,
    BLE_STATUS_LED_CONNECTED,
};

static const struct device *led_strip;
static struct led_rgb pixels[STRIP_NUM_PIXELS];
static enum ble_status_led_state last_state = BLE_STATUS_LED_UNKNOWN;
static bool last_visible;
static bool blink_visible;

static struct led_rgb off = {.r = 0, .g = 0, .b = 0};

static uint8_t brightness(void) {
    return (uint8_t)((CONFIG_ZMK_BLE_STATUS_LED_BRIGHTNESS * 255) / 100);
}

static enum ble_status_led_state read_ble_state(void) {
    if (zmk_ble_active_profile_is_connected()) {
        return BLE_STATUS_LED_CONNECTED;
    }

    if (zmk_ble_active_profile_is_open()) {
        return BLE_STATUS_LED_PAIRING;
    }

    return BLE_STATUS_LED_RECONNECTING;
}

static struct led_rgb color_for_state(enum ble_status_led_state state) {
    uint8_t b = brightness();

    switch (state) {
    case BLE_STATUS_LED_CONNECTED:
        return (struct led_rgb){.r = 0, .g = b, .b = 0};
    case BLE_STATUS_LED_PAIRING:
        return (struct led_rgb){.r = 0, .g = 0, .b = b};
    case BLE_STATUS_LED_RECONNECTING:
        return (struct led_rgb){.r = 0, .g = b, .b = b};
    default:
        return off;
    }
}

static bool state_is_blinking(enum ble_status_led_state state) {
    return state == BLE_STATUS_LED_PAIRING || state == BLE_STATUS_LED_RECONNECTING;
}

static int write_color(struct led_rgb color) {
    if (!led_strip) {
        return -ENODEV;
    }

    for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
        pixels[i] = color;
    }

    return led_strip_update_rgb(led_strip, pixels, STRIP_NUM_PIXELS);
}

static void ble_status_led_work_handler(struct k_work *work);
K_WORK_DELAYABLE_DEFINE(ble_status_led_work, ble_status_led_work_handler);

static void schedule_update(k_timeout_t delay) {
    k_work_reschedule(&ble_status_led_work, delay);
}

static void ble_status_led_work_handler(struct k_work *work) {
    enum ble_status_led_state state = read_ble_state();
    bool visible = true;

    if (state_is_blinking(state)) {
        blink_visible = !blink_visible;
        visible = blink_visible;
    }

    if (state != last_state || visible != last_visible) {
        int err = write_color(visible ? color_for_state(state) : off);
        if (err) {
            LOG_WRN("BLE status LED update failed: %d", err);
        }

        last_state = state;
        last_visible = visible;
    }

    schedule_update(K_MSEC(CONFIG_ZMK_BLE_STATUS_LED_POLL_INTERVAL_MS));
}

static int ble_status_led_listener(const zmk_event_t *eh) {
    schedule_update(K_NO_WAIT);
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(ble_status_led, ble_status_led_listener);
ZMK_SUBSCRIPTION(ble_status_led, zmk_ble_active_profile_changed);

static int ble_status_led_init(const struct device *_arg) {
    led_strip = device_get_binding(STRIP_LABEL);
    if (!led_strip) {
        LOG_ERR("BLE status LED strip not found: %s", STRIP_LABEL);
        return -ENODEV;
    }

    schedule_update(K_MSEC(1000));
    return 0;
}

SYS_INIT(ble_status_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
