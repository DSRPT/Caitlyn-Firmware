#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Caitlyn Core – Voice Engine + Push-to-Talk + Command Dispatch
 *
 * This module owns:
 *  - Push-to-Talk state machine
 *  - Listening overlay trigger
 *  - TinyML inference gating
 *  - Command parsing and chaining
 *  - High-level init of power / config / tamper
 */

/**
 * @brief Full system initialization
 * Call once after Bruce core has started.
 */
esp_err_t caitlyn_init(void);

/**
 * @brief Start the Caitlyn background tasks
 */
esp_err_t caitlyn_start(void);

/**
 * @brief Main periodic tick – call from Bruce main loop or a dedicated task
 */
void caitlyn_tick(void);

/**
 * @brief Called by the power module or button handler when PTT state changes
 */
void caitlyn_on_ptt_changed(bool pressed);

/**
 * @brief Force processing of the current audio buffer (used on button release)
 */
void caitlyn_process_audio_buffer(void);

/**
 * @brief Returns true if the listening overlay should be shown
 */
bool caitlyn_is_listening(void);

/**
 * @brief Register a command handler
 * name: lowercase command string (e.g. "deauth_all")
 * handler: function that executes the attack
 */
typedef void (*caitlyn_command_handler_t)(void);
esp_err_t caitlyn_register_command(const char *name, caitlyn_command_handler_t handler);

/**
 * @brief Execute a raw command string (supports simple chaining with "and" / "then")
 */
void caitlyn_execute_command_string(const char *cmd);

/**
 * @brief Get a human-readable status string for the UI
 */
const char *caitlyn_get_status_string(void);

#ifdef __cplusplus
}
#endif
