#include "tamper.h"
#include "caitlyn_config.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <string.h>

static const char *TAG = "tamper";

static tamper_callback_t s_callback = NULL;
static bool s_triggered = false;
static bool s_initialized = false;

// Triple-press detection
static int64_t s_press_times[3] = {0};
static int s_press_count = 0;
static const int64_t TRIPLE_PRESS_WINDOW_US = 800000;  // 800 ms

// Acoustic thresholds (adjusted by sensitivity 0–10)
static float s_loud_threshold = 85.0f;
static float s_silence_threshold = 5.0f;
static int64_t s_last_loud_us = 0;
static int64_t s_silence_start_us = 0;
static bool s_in_silence = false;

static void update_thresholds_from_config(void)
{
    uint8_t sens = caitlyn_config_get_tamper_sensitivity();
    // Higher sensitivity = lower loud threshold, higher silence threshold
    s_loud_threshold = 90.0f - (sens * 2.0f);      // 90 → 70
    s_silence_threshold = 3.0f + (sens * 0.5f);    // 3 → 8
}

static void do_trigger(const char *reason)
{
    if (s_triggered) {
        return;
    }
    s_triggered = true;
    ESP_LOGW(TAG, "TAMPER TRIGGERED: %s", reason);

    if (s_callback) {
        s_callback();
    }
}

esp_err_t tamper_init(tamper_callback_t cb)
{
    s_callback = cb;
    s_triggered = false;
    s_press_count = 0;
    memset(s_press_times, 0, sizeof(s_press_times));
    s_in_silence = false;
    update_thresholds_from_config();
    s_initialized = true;
    ESP_LOGI(TAG, "Tamper detection initialized (sensitivity=%d)", caitlyn_config_get_tamper_sensitivity());
    return ESP_OK;
}

void tamper_check_mic(float avg_volume)
{
    if (!s_initialized || s_triggered) {
        return;
    }

    update_thresholds_from_config();
    int64_t now = esp_timer_get_time();

    // Loud anomaly
    if (avg_volume > s_loud_threshold) {
        s_last_loud_us = now;
        s_in_silence = false;
        // Optional: require sustained loudness – for now single spike is enough
        do_trigger("Acoustic anomaly (loud)");
        return;
    }

    // Silence detection (possible covering of the device)
    if (avg_volume < s_silence_threshold) {
        if (!s_in_silence) {
            s_in_silence = true;
            s_silence_start_us = now;
        } else if ((now - s_silence_start_us) > 3000000) {  // 3 seconds of silence
            do_trigger("Acoustic anomaly (prolonged silence)");
        }
    } else {
        s_in_silence = false;
    }
}

void tamper_on_button_press(void)
{
    if (!s_initialized || s_triggered) {
        return;
    }

    int64_t now = esp_timer_get_time();

    // Shift previous presses
    if (s_press_count < 3) {
        s_press_times[s_press_count++] = now;
    } else {
        s_press_times[0] = s_press_times[1];
        s_press_times[1] = s_press_times[2];
        s_press_times[2] = now;
    }

    if (s_press_count >= 3) {
        int64_t window = s_press_times[2] - s_press_times[0];
        if (window <= TRIPLE_PRESS_WINDOW_US) {
            do_trigger("Triple button press");
            s_press_count = 0;  // reset
        }
    }

    // Expire old presses
    if (s_press_count > 0 && (now - s_press_times[0]) > TRIPLE_PRESS_WINDOW_US) {
        // shift out expired
        int new_count = 0;
        for (int i = 0; i < s_press_count; i++) {
            if ((now - s_press_times[i]) <= TRIPLE_PRESS_WINDOW_US) {
                s_press_times[new_count++] = s_press_times[i];
            }
        }
        s_press_count = new_count;
    }
}

void tamper_force_trigger(void)
{
    do_trigger("Forced (voice command or API)");
}

bool tamper_is_triggered(void)
{
    return s_triggered;
}
