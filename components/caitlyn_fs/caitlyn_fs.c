#include "caitlyn_fs.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_littlefs.h"
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

static const char *TAG = "caitlyn_fs";

static bool s_initialized = false;

static esp_err_t ensure_dir(const char *path)
{
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            return ESP_OK;
        }
        ESP_LOGW(TAG, "Path exists but is not a directory: %s", path);
        return ESP_ERR_INVALID_STATE;
    }

    // Create with recursive-ish approach (one level at a time is enough for our layout)
    if (mkdir(path, 0755) != 0) {
        ESP_LOGE(TAG, "Failed to create directory: %s", path);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Created directory: %s", path);
    return ESP_OK;
}

esp_err_t caitlyn_fs_init(void)
{
    if (s_initialized) {
        return ESP_OK;
    }

    // LittleFS side (always expected)
    ensure_dir(CAITLYN_FS_MODELS);
    ensure_dir(CAITLYN_FS_MACROS);
    ensure_dir(CAITLYN_FS_LOGS);
    ensure_dir(CAITLYN_FS_CONFIG);

    // SD side (optional – may not be mounted)
    struct stat st;
    if (stat(CAITLYN_FS_SD_BASE, &st) == 0 && S_ISDIR(st.st_mode)) {
        ensure_dir(CAITLYN_FS_PAYLOADS);
        ensure_dir(CAITLYN_FS_CAPTURES);
        ESP_LOGI(TAG, "SD card present – payload/capture dirs ready");
    } else {
        ESP_LOGW(TAG, "SD card not mounted – skipping /sdcard paths");
    }

    s_initialized = true;
    ESP_LOGI(TAG, "Caitlyn file system layout initialized");
    return ESP_OK;
}

bool caitlyn_fs_dir_exists(const char *path)
{
    if (!path) return false;
    struct stat st;
    if (stat(path, &st) != 0) return false;
    return S_ISDIR(st.st_mode);
}

esp_err_t caitlyn_fs_log_append(const char *filename, const char *line)
{
    if (!filename || !line) {
        return ESP_ERR_INVALID_ARG;
    }

    char path[160];
    snprintf(path, sizeof(path), "%s/%s", CAITLYN_FS_LOGS, filename);

    FILE *f = fopen(path, "a");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open log file: %s", path);
        return ESP_FAIL;
    }

    time_t now = time(NULL);
    struct tm t;
    localtime_r(&now, &t);
    fprintf(f, "%04d-%02d-%02d %02d:%02d:%02d  %s\n",
            t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
            t.tm_hour, t.tm_min, t.tm_sec, line);
    fclose(f);
    return ESP_OK;
}

esp_err_t caitlyn_fs_get_model_path(const char *model_name, char *out, size_t out_len)
{
    if (!model_name || !out || out_len < 32) {
        return ESP_ERR_INVALID_ARG;
    }
    snprintf(out, out_len, "%s/%s", CAITLYN_FS_MODELS, model_name);
    return ESP_OK;
}

static esp_err_t wipe_dir_contents(const char *dir_path)
{
    DIR *dir = opendir(dir_path);
    if (!dir) {
        return ESP_OK;  // nothing to wipe
    }

    struct dirent *entry;
    char full[200];
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        snprintf(full, sizeof(full), "%s/%s", dir_path, entry->d_name);
        unlink(full);  // best-effort
    }
    closedir(dir);
    return ESP_OK;
}

esp_err_t caitlyn_fs_secure_wipe(bool force_all)
{
    ESP_LOGW(TAG, "Secure wipe requested (force_all=%d)", force_all);

    wipe_dir_contents(CAITLYN_FS_LOGS);
    wipe_dir_contents(CAITLYN_FS_MACROS);

    if (force_all) {
        wipe_dir_contents(CAITLYN_FS_MODELS);
        wipe_dir_contents(CAITLYN_FS_CONFIG);
        // Also clear SD captures if present
        if (caitlyn_fs_dir_exists(CAITLYN_FS_CAPTURES)) {
            wipe_dir_contents(CAITLYN_FS_CAPTURES);
        }
    }

    ESP_LOGI(TAG, "Secure wipe complete");
    return ESP_OK;
}
