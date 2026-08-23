/**
 * @file caitlyn_bootstrap.c
 * @brief Recommended single entry point for integrating Caitlyn into Bruce
 *
 * Copy this file (or the function) into your main application and call
 * caitlyn_bootstrap() after Bruce has finished hardware + LVGL initialization.
 */

#include "caitlyn.h"
#include "caitlyn_ui.h"
#include "caitlyn_fs.h"
#include "caitlyn_commands.h"
#include "caitlyn_ota.h"
#include "caitlyn_nrf24.h"
#include "caitlyn_skills.h"
#include "caitlyn_power.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "caitlyn_boot";

/**
 * @brief Full Caitlyn bootstrap
 * Call once after Bruce core is ready.
 */
esp_err_t caitlyn_bootstrap(void)
{
    ESP_LOGI(TAG, "=== Caitlyn bootstrap starting ===");

    // 1. File system layout
    ESP_ERROR_CHECK(caitlyn_fs_init());

    // 2. Core (config → power → tamper)
    ESP_ERROR_CHECK(caitlyn_init());

    // 3. UI helpers
    ESP_ERROR_CHECK(caitlyn_ui_init());

    // 4. OTA / model manager
    ESP_ERROR_CHECK(caitlyn_ota_init());

    // 5. nRF24 expansion (safe even if no modules attached)
    ESP_ERROR_CHECK(caitlyn_nrf24_init());

    // 6. Register all built-in atomic commands
    ESP_ERROR_CHECK(caitlyn_commands_register_all());

    // 7. Load and register pre-loaded multi-step skills
    ESP_ERROR_CHECK(caitlyn_skills_init());

    // 8. Start the core state machine
    ESP_ERROR_CHECK(caitlyn_start());

    // Optional: first-boot legal confirmation
    if (caitlyn_config_is_first_boot()) {
        ESP_LOGW(TAG, "First boot detected – show authorized-use screen here");
        // UI code should display a confirmation dialog
        // After user accepts:
        // caitlyn_config_set_first_boot_done();
        // caitlyn_config_save();
    }

    ESP_LOGI(TAG, "=== Caitlyn bootstrap complete ===");
    ESP_LOGI(TAG, "Status: %s", caitlyn_get_status_string());
    ESP_LOGI(TAG, "Voice mode: %s",
             caitlyn_config_get_voice_mode() == CAITLYN_VOICE_MODE_PTT ? "PTT" : "Wake-Word");
    ESP_LOGI(TAG, "nRF24 modules detected: %d", caitlyn_nrf24_get_module_count());
    ESP_LOGI(TAG, "Pre-loaded skills: %d", caitlyn_skills_count());

    return ESP_OK;
}

/**
 * @brief Example FreeRTOS task that keeps Caitlyn alive
 * You can also call the ticks from Bruce's existing main loop.
 */
void caitlyn_task(void *pvParameters)
{
    (void)pvParameters;
    while (1) {
        caitlyn_tick();
        caitlyn_ui_tooltip_tick();

        // Optional: feed mic volume for acoustic tamper
        // float vol = get_mic_average_volume();
        // tamper_check_mic(vol);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/**
 * @brief Helper to start the Caitlyn background task
 */
void caitlyn_start_background_task(void)
{
    xTaskCreate(caitlyn_task, "caitlyn", 4096, NULL, 5, NULL);
}
