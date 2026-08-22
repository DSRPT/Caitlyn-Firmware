#include "caitlyn_ota.h"
#include "caitlyn_config.h"
#include "caitlyn_fs.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_crt_bundle.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

static const char *TAG = "caitlyn_ota";
static bool s_initialized = false;

esp_err_t caitlyn_ota_init(void)
{
    if (s_initialized) return ESP_OK;
    // Ensure models directory exists
    caitlyn_fs_init();
    s_initialized = true;
    ESP_LOGI(TAG, "OTA / model manager initialized");
    return ESP_OK;
}

esp_err_t caitlyn_ota_download_model(const char *url, const char *model_name)
{
    if (!url || !model_name) {
        return ESP_ERR_INVALID_ARG;
    }

    char local_path[160];
    snprintf(local_path, sizeof(local_path), "%s/%s", CAITLYN_FS_MODELS, model_name);

    ESP_LOGI(TAG, "Downloading model from %s -> %s", url, local_path);

    esp_http_client_config_t config = {
        .url = url,
        .timeout_ms = 30000,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        return ESP_FAIL;
    }

    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return err;
    }

    int content_length = esp_http_client_fetch_headers(client);
    if (content_length <= 0) {
        ESP_LOGW(TAG, "Content-Length unknown or zero");
    }

    FILE *f = fopen(local_path, "wb");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open local file for writing");
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return ESP_FAIL;
    }

    char buffer[1024];
    int total = 0;
    while (1) {
        int read = esp_http_client_read(client, buffer, sizeof(buffer));
        if (read < 0) {
            ESP_LOGE(TAG, "HTTP read error");
            err = ESP_FAIL;
            break;
        }
        if (read == 0) break;
        fwrite(buffer, 1, read, f);
        total += read;
    }

    fclose(f);
    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Model downloaded successfully (%d bytes)", total);
    } else {
        // Clean up partial file
        unlink(local_path);
    }
    return err;
}

esp_err_t caitlyn_ota_set_active_model(const char *model_name)
{
    if (!model_name) return ESP_ERR_INVALID_ARG;

    char full[160];
    snprintf(full, sizeof(full), "%s/%s", CAITLYN_FS_MODELS, model_name);

    // Basic existence check
    struct stat st;
    if (stat(full, &st) != 0) {
        ESP_LOGE(TAG, "Model file does not exist: %s", full);
        return ESP_ERR_NOT_FOUND;
    }

    ESP_LOGI(TAG, "Setting active model to: %s", full);
    // TODO: add caitlyn_config_set_model_path() if not already present
    return ESP_OK;
}

const char *caitlyn_ota_get_active_model(void)
{
    return caitlyn_config_get()->preferred_model_path;
}

esp_err_t caitlyn_ota_list_models(char *buf, size_t buf_len)
{
    if (!buf || buf_len < 16) return ESP_ERR_INVALID_ARG;

    buf[0] = '\0';
    DIR *dir = opendir(CAITLYN_FS_MODELS);
    if (!dir) {
        return ESP_OK;  // empty
    }

    struct dirent *entry;
    size_t used = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        size_t len = strlen(entry->d_name);
        if (used + len + 2 >= buf_len) break;
        if (used > 0) {
            buf[used++] = '\n';
        }
        memcpy(buf + used, entry->d_name, len);
        used += len;
        buf[used] = '\0';
    }
    closedir(dir);
    return ESP_OK;
}

esp_err_t caitlyn_ota_firmware(const char *url)
{
    if (!url) return ESP_ERR_INVALID_ARG;

    ESP_LOGI(TAG, "Starting firmware OTA from %s", url);

    esp_http_client_config_t config = {
        .url = url,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .timeout_ms = 60000,
    };
    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };

    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Firmware OTA successful – rebooting");
        esp_restart();
    } else {
        ESP_LOGE(TAG, "Firmware OTA failed: %s", esp_err_to_name(ret));
    }
    return ret;
}
