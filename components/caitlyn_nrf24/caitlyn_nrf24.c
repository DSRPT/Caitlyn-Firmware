#include "caitlyn_nrf24.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include <string.h>

static const char *TAG = "caitlyn_nrf24";

// Example CS pins for up to 3 modules (adjust to free GPIOs on T-Embed)
// These are placeholders – measure the actual free pins on your board.
#define NRF24_CS_0   10
#define NRF24_CS_1   11
#define NRF24_CS_2   12
#define NRF24_CE     13   // shared CE or individual as needed

static int s_module_count = 0;
static bool s_initialized = false;
static bool s_active = false;

esp_err_t caitlyn_nrf24_init(void)
{
    if (s_initialized) return ESP_OK;

    // Placeholder detection logic.
    // In a real implementation you would:
    // 1. Configure SPI bus (shared with other devices if necessary)
    // 2. Probe each CS pin by reading the nRF24 STATUS register
    // 3. Count how many respond with a valid ID

    s_module_count = 0;  // start with zero until hardware is attached
    s_active = false;
    s_initialized = true;

    ESP_LOGI(TAG, "nRF24 subsystem initialized (detected modules: %d)", s_module_count);
    ESP_LOGI(TAG, "Attach 1–3 nRF24L01+ modules and update CS pins in caitlyn_nrf24.c");
    return ESP_OK;
}

int caitlyn_nrf24_get_module_count(void)
{
    return s_module_count;
}

esp_err_t caitlyn_nrf24_start_scan(void)
{
    if (!s_initialized) return ESP_ERR_INVALID_STATE;
    if (s_module_count == 0) {
        ESP_LOGW(TAG, "No nRF24 modules present – scan skipped");
        return ESP_ERR_NOT_FOUND;
    }

    s_active = true;
    ESP_LOGI(TAG, "Starting 2.4 GHz scan on %d module(s)", s_module_count);
    // TODO: implement channel hopping + RSSI sampling
    return ESP_OK;
}

esp_err_t caitlyn_nrf24_stop(void)
{
    s_active = false;
    ESP_LOGI(TAG, "nRF24 activity stopped");
    return ESP_OK;
}

esp_err_t caitlyn_nrf24_jam_channel(uint8_t channel)
{
    if (!s_initialized) return ESP_ERR_INVALID_STATE;
    if (s_module_count == 0) {
        ESP_LOGW(TAG, "No nRF24 modules – jam not possible");
        return ESP_ERR_NOT_FOUND;
    }
    if (channel > 125) channel = 125;

    s_active = true;
    ESP_LOGI(TAG, "Jamming 2.4 GHz channel %u (low duty)", channel);
    // TODO: continuous carrier or noise on the selected channel
    return ESP_OK;
}

esp_err_t caitlyn_nrf24_mousejack_inject(const uint8_t *payload, size_t len)
{
    if (!payload || len == 0) return ESP_ERR_INVALID_ARG;
    if (s_module_count == 0) {
        ESP_LOGW(TAG, "No nRF24 modules – MouseJack not possible");
        return ESP_ERR_NOT_FOUND;
    }

    ESP_LOGI(TAG, "MouseJack inject (%u bytes) – placeholder", (unsigned)len);
    // TODO: format and transmit Enhanced ShockBurst HID payload
    return ESP_OK;
}

bool caitlyn_nrf24_is_active(void)
{
    return s_active;
}
