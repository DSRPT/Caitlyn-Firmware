#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Caitlyn UI helpers (LVGL overlays + tooltips)
 *
 * These functions are designed to be called from the main LVGL task.
 * They assume LVGL is already initialized by Bruce.
 */

/**
 * @brief Initialize UI helpers
 */
esp_err_t caitlyn_ui_init(void);

/**
 * @brief Show or hide the "CAITLYN LISTENING..." overlay
 */
void caitlyn_ui_set_listening_overlay(bool show);

/**
 * @brief Update the status text on the listening overlay (if visible)
 */
void caitlyn_ui_set_status_text(const char *text);

/**
 * @brief Start the 2-second tooltip timer for the currently focused menu item
 * Call this when a menu item receives focus.
 */
void caitlyn_ui_start_tooltip_timer(const char *tooltip_text);

/**
 * @brief Cancel any pending tooltip
 */
void caitlyn_ui_cancel_tooltip(void);

/**
 * @brief Call from LVGL timer or main UI loop to handle tooltip timeout
 */
void caitlyn_ui_tooltip_tick(void);

#ifdef __cplusplus
}
#endif
