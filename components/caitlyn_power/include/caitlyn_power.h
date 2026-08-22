#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Caitlyn Power Management
 *
 * Handles:
 *  - Screen timeout / backlight control
 *  - Deep sleep entry and wake sources
 *  - PTT (Push-to-Talk) button state tracking
 *  - Activity reset for timeouts
 *  - Low-power helpers
 *
 * Center button is expected on GPIO 0 (active low) on T-Embed.
 */

/**
 * @brief Initialize power management subsystem
 * Must be called after caitlyn_config_init()
 */
esp_err_t caitlyn_power_init(void);

/**
 * @brief Periodic tick – call from main loop or a FreeRTOS timer (e.g. every 100–250 ms)
 * Handles screen timeout logic.
 */
void caitlyn_power_tick(void);

/**
 * @brief Call this on any user activity (button press, encoder turn, voice command, etc.)
 * Resets the screen timeout timer.
 */
void caitlyn_power_reset_activity(void);

/**
 * @brief Request the device to enter deep sleep
 * Wakes on center button press (GPIO 0) or other configured sources.
 */
void caitlyn_power_request_sleep(void);

/**
 * @brief Returns true while the center button is currently held (PTT active)
 */
bool caitlyn_power_is_ptt_active(void);

/**
 * @brief Force screen on/off (overrides timeout temporarily)
 */
void caitlyn_power_set_screen_on(bool on);

/**
 * @brief Returns true if screen is currently considered on
 */
bool caitlyn_power_is_screen_on(void);

/**
 * @brief Get current battery voltage in millivolts (if fuel gauge available)
 * Returns 0 if not available.
 */
uint32_t caitlyn_power_get_battery_mv(void);

/**
 * @brief Returns true if battery is considered low
 */
bool caitlyn_power_is_battery_low(void);

#ifdef __cplusplus
}
#endif
