#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Caitlyn Tamper Detection
 *
 * Two independent triggers:
 *  1. Acoustic anomaly (sudden loud noise or prolonged silence via mic)
 *  2. Triple-press of the center button within a short window
 *
 * On trigger: calls the registered callback (usually screen off + secure wipe).
 */

typedef void (*tamper_callback_t)(void);

/**
 * @brief Initialize tamper detection
 * @param cb  Function to call when tamper is detected (can be NULL)
 */
esp_err_t tamper_init(tamper_callback_t cb);

/**
 * @brief Call periodically with current average mic volume (0.0 – 100.0 scale)
 * Used for acoustic anomaly detection.
 */
void tamper_check_mic(float avg_volume);

/**
 * @brief Call from the button / power tick when the center button is pressed
 * Handles the triple-press detection.
 */
void tamper_on_button_press(void);

/**
 * @brief Force a tamper action (e.g. from voice command "self destruct")
 */
void tamper_force_trigger(void);

/**
 * @brief Returns true if tamper has been triggered and action is pending/complete
 */
bool tamper_is_triggered(void);

#ifdef __cplusplus
}
#endif
