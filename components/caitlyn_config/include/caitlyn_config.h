#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Caitlyn Configuration (NVS-backed)
 *
 * Stores user preferences that must survive reboots.
 * Namespace: "caitlyn"
 */

typedef enum {
    CAITLYN_VOICE_MODE_PTT = 0,      // Push-to-Talk (default, best battery)
    CAITLYN_VOICE_MODE_WAKEWORD = 1  // Continuous "Hey Caitlyn"
} caitlyn_voice_mode_t;

typedef struct {
    caitlyn_voice_mode_t voice_mode;
    float                confidence_threshold;  // 0.0 – 1.0 (default 0.85)
    uint32_t             tooltip_delay_ms;      // default 2000
    uint8_t              tamper_sensitivity;    // 0–10 (default 5)
    char                 preferred_model_path[64];
    uint32_t             screen_timeout_s;      // default 30
    int8_t               tx_power_level;        // radio TX power preference
    bool                 first_boot_done;
} caitlyn_config_t;

/**
 * @brief Initialize the configuration subsystem (call once at boot)
 */
esp_err_t caitlyn_config_init(void);

/**
 * @brief Load configuration from NVS into RAM
 */
esp_err_t caitlyn_config_load(void);

/**
 * @brief Save current RAM configuration to NVS
 */
esp_err_t caitlyn_config_save(void);

/**
 * @brief Get a pointer to the current live config (do not free)
 */
const caitlyn_config_t *caitlyn_config_get(void);

// Convenience getters / setters
caitlyn_voice_mode_t caitlyn_config_get_voice_mode(void);
void                 caitlyn_config_set_voice_mode(caitlyn_voice_mode_t mode);

float caitlyn_config_get_confidence(void);
void  caitlyn_config_set_confidence(float value);

uint32_t caitlyn_config_get_tooltip_delay(void);
void     caitlyn_config_set_tooltip_delay(uint32_t ms);

uint8_t caitlyn_config_get_tamper_sensitivity(void);
void    caitlyn_config_set_tamper_sensitivity(uint8_t value);

uint32_t caitlyn_config_get_screen_timeout(void);
void     caitlyn_config_set_screen_timeout(uint32_t seconds);

bool caitlyn_config_is_first_boot(void);
void caitlyn_config_set_first_boot_done(void);

#ifdef __cplusplus
}
#endif
