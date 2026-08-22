#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Register all built-in Caitlyn command handlers
 *
 * Call this after caitlyn_init() and after Bruce attack modules are ready.
 * Each handler is a thin wrapper that should call the real Bruce function.
 */
esp_err_t caitlyn_commands_register_all(void);

// Individual handlers (can also be registered manually)
void caitlyn_cmd_deauth_all(void);
void caitlyn_cmd_deauth_target(void);
void caitlyn_cmd_ble_spam(void);
void caitlyn_cmd_spam_all(void);
void caitlyn_cmd_subghz_jam(void);
void caitlyn_cmd_subghz_replay(void);
void caitlyn_cmd_evil_portal(void);
void caitlyn_cmd_badble_now(void);
void caitlyn_cmd_scan_all(void);
void caitlyn_cmd_stealth_mode(void);
void caitlyn_cmd_wipe_logs(void);
void caitlyn_cmd_help(void);
void caitlyn_cmd_self_destruct(void);
void caitlyn_cmd_conference_takeover(void);
void caitlyn_cmd_lockdown(void);

#ifdef __cplusplus
}
#endif
