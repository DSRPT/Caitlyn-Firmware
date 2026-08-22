#include "caitlyn_power.h"
#include "caitlyn_config.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "caitlyn_power";

// T-Embed center button is GPIO 0 (active low)
#define CAITLYN_BUTTON_GPIO          0
#define CAITLYN_BUTTON_ACTIVE_LEVEL  0

static bool s_initialized = false;
static bool s_screen_on = true;
static bool s_ptt_active = false;
static int64_t s_last_activity_us = 0;
static int64_t s_screen_timeout_us = 30 * 1000000LL;  // default, updated from config

// Simple debounce for button
static int64_t s_last_button_change_us = 0;
static bool s_last_button_raw = false;

static void update_timeout_from_config(void)
{
    uint32_t timeout_s = caitlyn_config_get_screen_timeout();
    if (timeout_s == 0) {
        timeout_s = 30;  // safety
    }
    s_screen_timeout_us = (int64_t)timeout_s * 1000000LL;
}

static bool read_button_raw(void)
{
    return gpio_get_level(CAITLYN_BUTTON_GPIO) == CAITLYN_BUTTON_ACTIVE_LEVEL;
}

static void button_isr_handler(void *arg)
{
    // We only use the ISR to wake from light sleep / note activity.
    // Full PTT state is polled in the tick for simplicity and reliability.
    (void)arg;
}

esp_err_t caitlyn_power_init(void)
{
    if (s_initialized) {
        return ESP_OK;
    }

    // Configure center button as input with pull-up
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << CAITLYN_BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    esp_err_t err = gpio_config(&io_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "gpio_config failed: %s", esp_err_to_name(err));
        return err;
    }

    // Install ISR service (may already be installed by Bruce)
    gpio_install_isr_service(0);
    gpio_isr_handler_add(CAITLYN_BUTTON_GPIO, button_isr_handler, NULL);

    // Enable button as wake source for deep sleep
    gpio_wakeup_enable(CAITLYN_BUTTON_GPIO, CAITLYN_BUTTON_ACTIVE_LEVEL == 0 ?
                       GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);
    esp_sleep_enable_gpio_wakeup();

    update_timeout_from_config();
    s_last_activity_us = esp_timer_get_time();
    s_screen_on = true;
    s_ptt_active = false;
    s_initialized = true;

    ESP_LOGI(TAG, "Power management initialized (button GPIO %d, timeout %llu s)",
             CAITLYN_BUTTON_GPIO, s_screen_timeout_us / 1000000ULL);
    return ESP_OK;
}

void caitlyn_power_tick(void)
{
    if (!s_initialized) {
        return;
    }

    // Update timeout in case config changed
    update_timeout_from_config();

    // Debounced PTT state
    bool raw = read_button_raw();
    int64_t now = esp_timer_get_time();

    if (raw != s_last_button_raw) {
        s_last_button_change_us = now;
        s_last_button_raw = raw;
    }

    // 30 ms debounce
    if ((now - s_last_button_change_us) > 30000) {
        s_ptt_active = raw;
        if (s_ptt_active) {
            caitlyn_power_reset_activity();
        }
    }

    // Screen timeout logic
    if (s_screen_on && (now - s_last_activity_us) > s_screen_timeout_us) {
        ESP_LOGI(TAG, "Screen timeout reached – turning screen off");
        caitlyn_power_set_screen_on(false);
    }
}

void caitlyn_power_reset_activity(void)
{
    s_last_activity_us = esp_timer_get_time();
    if (!s_screen_on) {
        caitlyn_power_set_screen_on(true);
    }
}

void caitlyn_power_request_sleep(void)
{
    ESP_LOGI(TAG, "Entering deep sleep (wake on button GPIO %d)", CAITLYN_BUTTON_GPIO);

    // Turn screen off before sleeping
    caitlyn_power_set_screen_on(false);

    // Flush logs
    vTaskDelay(pdMS_TO_TICKS(50));

    esp_deep_sleep_start();
}

bool caitlyn_power_is_ptt_active(void)
{
    return s_ptt_active;
}

void caitlyn_power_set_screen_on(bool on)
{
    s_screen_on = on;

    // NOTE: Actual backlight / display control is board-specific.
    // On T-Embed this is normally handled by Bruce's display driver.
    // We expose the logical state so higher layers (LVGL, menus) can react.
    // Example integration point:
    //   if (on) display_set_brightness(80);
    //   else    display_set_brightness(0);

    ESP_LOGD(TAG, "Screen logical state: %s", on ? "ON" : "OFF");
}

bool caitlyn_power_is_screen_on(void)
{
    return s_screen_on;
}

uint32_t caitlyn_power_get_battery_mv(void)
{
    // T-Embed CC1101 has BQ27220 fuel gauge on some variants.
    // Placeholder – integrate real driver when available.
    return 0;
}

bool caitlyn_power_is_battery_low(void)
{
    uint32_t mv = caitlyn_power_get_battery_mv();
    if (mv == 0) {
        return false;  // unknown
    }
    return mv < 3400;  // roughly 3.4 V
}
