#include "caitlyn_ui.h"
#include "caitlyn_config.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <string.h>

static const char *TAG = "caitlyn_ui";

static bool s_initialized = false;
static bool s_overlay_visible = false;
static char s_status_text[64] = "CAITLYN LISTENING...";

// Tooltip state
static bool s_tooltip_pending = false;
static int64_t s_tooltip_start_us = 0;
static char s_tooltip_text[128] = {0};
static bool s_tooltip_visible = false;

esp_err_t caitlyn_ui_init(void)
{
    s_initialized = true;
    s_overlay_visible = false;
    s_tooltip_pending = false;
    s_tooltip_visible = false;
    ESP_LOGI(TAG, "Caitlyn UI helpers initialized");
    return ESP_OK;
}

void caitlyn_ui_set_listening_overlay(bool show)
{
    if (!s_initialized) return;

    s_overlay_visible = show;
    if (show) {
        ESP_LOGI(TAG, "Showing listening overlay: %s", s_status_text);
        // Real implementation would create/show an LVGL object:
        // lv_obj_t *overlay = lv_obj_create(lv_scr_act());
        // lv_label_set_text(...);
        // style it blue, centered, etc.
    } else {
        ESP_LOGI(TAG, "Hiding listening overlay");
        // lv_obj_del(overlay);
    }
}

void caitlyn_ui_set_status_text(const char *text)
{
    if (!text) return;
    strncpy(s_status_text, text, sizeof(s_status_text) - 1);
    s_status_text[sizeof(s_status_text) - 1] = '\0';

    if (s_overlay_visible) {
        // Update the live label if the overlay is currently shown
        ESP_LOGD(TAG, "Overlay status updated: %s", s_status_text);
    }
}

void caitlyn_ui_start_tooltip_timer(const char *tooltip_text)
{
    if (!tooltip_text) return;

    strncpy(s_tooltip_text, tooltip_text, sizeof(s_tooltip_text) - 1);
    s_tooltip_text[sizeof(s_tooltip_text) - 1] = '\0';
    s_tooltip_start_us = esp_timer_get_time();
    s_tooltip_pending = true;
    s_tooltip_visible = false;
}

void caitlyn_ui_cancel_tooltip(void)
{
    s_tooltip_pending = false;
    s_tooltip_visible = false;
    // Hide any existing tooltip object
}

void caitlyn_ui_tooltip_tick(void)
{
    if (!s_initialized || !s_tooltip_pending) {
        return;
    }

    uint32_t delay_ms = caitlyn_config_get_tooltip_delay();
    int64_t elapsed = esp_timer_get_time() - s_tooltip_start_us;

    if (elapsed >= (int64_t)delay_ms * 1000) {
        s_tooltip_pending = false;
        s_tooltip_visible = true;
        ESP_LOGI(TAG, "Showing tooltip: %s", s_tooltip_text);
        // Real implementation:
        // Create a small scrolling LVGL label at the bottom of the screen
        // lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    }
}
