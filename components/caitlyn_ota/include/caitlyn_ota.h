#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Caitlyn OTA / Model Management
 *
 * Handles:
 *  - Downloading new voice models (.tflite) over HTTP
 *  - Storing them under /littlefs/models/
 *  - Hot-swapping the active model path in config
 *  - Optional firmware OTA (via esp_https_ota) for future use
 */

/**
 * @brief Initialize the OTA / model manager
 */
esp_err_t caitlyn_ota_init(void);

/**
 * @brief Download a new model from a URL and save it under /littlefs/models/
 *
 * @param url          Full HTTP(S) URL of the .tflite file
 * @param model_name   Filename to save as (e.g. "model_v2.tflite")
 * @return ESP_OK on success
 */
esp_err_t caitlyn_ota_download_model(const char *url, const char *model_name);

/**
 * @brief Set the active model path and save to NVS
 * After this call the next inference will use the new model.
 */
esp_err_t caitlyn_ota_set_active_model(const char *model_name);

/**
 * @brief Get the currently active model path (from config)
 */
const char *caitlyn_ota_get_active_model(void);

/**
 * @brief List available models in /littlefs/models/
 * Fills a simple null-terminated array of filenames (caller provides buffer).
 */
esp_err_t caitlyn_ota_list_models(char *buf, size_t buf_len);

/**
 * @brief Perform a full firmware OTA from a URL (advanced)
 * Uses esp_https_ota. Requires a properly signed image.
 */
esp_err_t caitlyn_ota_firmware(const char *url);

#ifdef __cplusplus
}
#endif
