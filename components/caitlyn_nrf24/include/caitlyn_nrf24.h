#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Caitlyn nRF24 Expansion (NyanBOX-style)
 *
 * Skeleton for multi-nRF24 support on the T-Embed.
 * Designed so that 1–3 external nRF24L01+ modules can be attached
 * via SPI (shared bus + individual CS pins).
 *
 * Features planned:
 *  - Parallel scanning / jamming
 *  - MouseJacking payload injection
 *  - 2.4 GHz spectrum sampling
 *  - AirTag / tracker RSSI tracking
 */

typedef enum {
    CAITLYN_NRF24_ROLE_SCANNER = 0,
    CAITLYN_NRF24_ROLE_JAMMER,
    CAITLYN_NRF24_ROLE_MOUSEJACK,
    CAITLYN_NRF24_ROLE_TRACKER
} caitlyn_nrf24_role_t;

/**
 * @brief Initialize the nRF24 subsystem
 * Detects how many modules are present (0–3).
 */
esp_err_t caitlyn_nrf24_init(void);

/**
 * @brief Returns the number of detected nRF24 modules
 */
int caitlyn_nrf24_get_module_count(void);

/**
 * @brief Start a simple 2.4 GHz scan on the available modules
 */
esp_err_t caitlyn_nrf24_start_scan(void);

/**
 * @brief Stop any ongoing scan / jam activity
 */
esp_err_t caitlyn_nrf24_stop(void);

/**
 * @brief Start a low-duty jammer on the given channel (0–125)
 * Only active if at least one module is present.
 */
esp_err_t caitlyn_nrf24_jam_channel(uint8_t channel);

/**
 * @brief MouseJack placeholder – inject a short HID sequence
 * (Full implementation requires the specific nRF24 driver + payload format)
 */
esp_err_t caitlyn_nrf24_mousejack_inject(const uint8_t *payload, size_t len);

/**
 * @brief Returns true if the nRF24 subsystem is active
 */
bool caitlyn_nrf24_is_active(void);

#ifdef __cplusplus
}
#endif
