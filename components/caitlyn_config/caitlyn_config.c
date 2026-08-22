#include "caitlyn_config.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "caitlyn_config";
static const char *NVS_NAMESPACE = "caitlyn";

static caitlyn_config_t s_config;
static bool s_initialized = false;

// Default values
static const caitlyn_config_t DEFAULT_CONFIG = {
    .voice_mode = CAITLYN_VOICE_MODE_PTT,
    .confidence_threshold = 0.85f,
    .tooltip_delay_ms = 2000,
    .tamper_sensitivity = 5,
    .preferred_model_path = "/littlefs/models/model.tflite",
    .screen_timeout_s = 30,
    .tx_power_level = 0,   // 0 = default / auto
    .first_boot_done = false
};

esp_err_t caitlyn_config_init(void)
{
    if (s_initialized) {
        return ESP_OK;
    }

    // Ensure NVS is initialized (Bruce normally does this, but be safe)
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was truncated, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_flash_init failed: %s", esp_err_to_name(err));
        return err;
    }

    // Start with defaults
    memcpy(&s_config, &DEFAULT_CONFIG, sizeof(s_config));

    // Try to load saved values
    err = caitlyn_config_load();
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "Failed to load config, using defaults");
    }

    s_initialized = true;
    ESP_LOGI(TAG, "Config initialized (voice_mode=%d, confidence=%.2f, timeout=%lus)",
             s_config.voice_mode, s_config.confidence_threshold, s_config.screen_timeout_s);
    return ESP_OK;
}

esp_err_t caitlyn_config_load(void)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }

    // Load each field (ignore missing keys – keep defaults)
    int32_t tmp_i32;
    if (nvs_get_i32(handle, "voice_mode", &tmp_i32) == ESP_OK) {
        s_config.voice_mode = (caitlyn_voice_mode_t)tmp_i32;
    }

    if (nvs_get_i32(handle, "tooltip_delay", &tmp_i32) == ESP_OK) {
        s_config.tooltip_delay_ms = (uint32_t)tmp_i32;
    }

    if (nvs_get_i32(handle, "screen_timeout", &tmp_i32) == ESP_OK) {
        s_config.screen_timeout_s = (uint32_t)tmp_i32;
    }

    if (nvs_get_i32(handle, "tamper_sens", &tmp_i32) == ESP_OK) {
        s_config.tamper_sensitivity = (uint8_t)tmp_i32;
    }

    if (nvs_get_i32(handle, "tx_power", &tmp_i32) == ESP_OK) {
        s_config.tx_power_level = (int8_t)tmp_i32;
    }

    uint8_t tmp_u8;
    if (nvs_get_u8(handle, "first_boot", &tmp_u8) == ESP_OK) {
        s_config.first_boot_done = (tmp_u8 != 0);
    }

    // Confidence is stored as int (x1000)
    if (nvs_get_i32(handle, "confidence", &tmp_i32) == ESP_OK) {
        s_config.confidence_threshold = (float)tmp_i32 / 1000.0f;
    }

    size_t required_size = sizeof(s_config.preferred_model_path);
    nvs_get_str(handle, "model_path", s_config.preferred_model_path, &required_size);

    nvs_close(handle);
    ESP_LOGI(TAG, "Config loaded from NVS");
    return ESP_OK;
}

esp_err_t caitlyn_config_save(void)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS for writing: %s", esp_err_to_name(err));
        return err;
    }

    nvs_set_i32(handle, "voice_mode", (int32_t)s_config.voice_mode);
    nvs_set_i32(handle, "tooltip_delay", (int32_t)s_config.tooltip_delay_ms);
    nvs_set_i32(handle, "screen_timeout", (int32_t)s_config.screen_timeout_s);
    nvs_set_i32(handle, "tamper_sens", (int32_t)s_config.tamper_sensitivity);
    nvs_set_i32(handle, "tx_power", (int32_t)s_config.tx_power_level);
    nvs_set_u8(handle, "first_boot", s_config.first_boot_done ? 1 : 0);
    nvs_set_i32(handle, "confidence", (int32_t)(s_config.confidence_threshold * 1000.0f));
    nvs_set_str(handle, "model_path", s_config.preferred_model_path);

    err = nvs_commit(handle);
    nvs_close(handle);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Config saved to NVS");
    } else {
        ESP_LOGE(TAG, "nvs_commit failed: %s", esp_err_to_name(err));
    }
    return err;
}

const caitlyn_config_t *caitlyn_config_get(void)
{
    return &s_config;
}

caitlyn_voice_mode_t caitlyn_config_get_voice_mode(void)
{
    return s_config.voice_mode;
}

void caitlyn_config_set_voice_mode(caitlyn_voice_mode_t mode)
{
    s_config.voice_mode = mode;
}

float caitlyn_config_get_confidence(void)
{
    return s_config.confidence_threshold;
}

void caitlyn_config_set_confidence(float value)
{
    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;
    s_config.confidence_threshold = value;
}

uint32_t caitlyn_config_get_tooltip_delay(void)
{
    return s_config.tooltip_delay_ms;
}

void caitlyn_config_set_tooltip_delay(uint32_t ms)
{
    s_config.tooltip_delay_ms = ms;
}

uint8_t caitlyn_config_get_tamper_sensitivity(void)
{
    return s_config.tamper_sensitivity;
}

void caitlyn_config_set_tamper_sensitivity(uint8_t value)
{
    if (value > 10) value = 10;
    s_config.tamper_sensitivity = value;
}

uint32_t caitlyn_config_get_screen_timeout(void)
{
    return s_config.screen_timeout_s;
}

void caitlyn_config_set_screen_timeout(uint32_t seconds)
{
    s_config.screen_timeout_s = seconds;
}

bool caitlyn_config_is_first_boot(void)
{
    return !s_config.first_boot_done;
}

void caitlyn_config_set_first_boot_done(void)
{
    s_config.first_boot_done = true;
}
