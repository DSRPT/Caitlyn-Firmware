#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Caitlyn File System Helpers
 *
 * Creates and manages the standard directory layout on LittleFS / SD.
 *
 * Layout:
 *   /littlefs/models/
 *   /littlefs/macros/
 *   /littlefs/logs/
 *   /littlefs/config/
 *   /sdcard/payloads/
 *   /sdcard/captures/
 */

#define CAITLYN_FS_LITTLEFS_BASE   "/littlefs"
#define CAITLYN_FS_SD_BASE         "/sdcard"

#define CAITLYN_FS_MODELS         "/littlefs/models"
#define CAITLYN_FS_MACROS         "/littlefs/macros"
#define CAITLYN_FS_LOGS           "/littlefs/logs"
#define CAITLYN_FS_CONFIG         "/littlefs/config"
#define CAITLYN_FS_PAYLOADS       "/sdcard/payloads"
#define CAITLYN_FS_CAPTURES       "/sdcard/captures"

/**
 * @brief Initialize the Caitlyn directory structure.
 * Creates missing directories on both LittleFS and SD (if present).
 */
esp_err_t caitlyn_fs_init(void);

/**
 * @brief Returns true if the given path exists and is a directory
 */
bool caitlyn_fs_dir_exists(const char *path);

/**
 * @brief Append a line to a log file under /littlefs/logs/
 * Filename is relative (e.g. "attacks.log")
 */
esp_err_t caitlyn_fs_log_append(const char *filename, const char *line);

/**
 * @brief Get the full path for a model file
 * result buffer should be at least 128 bytes
 */
esp_err_t caitlyn_fs_get_model_path(const char *model_name, char *out, size_t out_len);

/**
 * @brief Securely wipe logs and macros (used by tamper self-destruct)
 * Does NOT wipe models or config unless force_all is true.
 */
esp_err_t caitlyn_fs_secure_wipe(bool force_all);

#ifdef __cplusplus
}
#endif
