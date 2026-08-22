#include "caitlyn_commands.h"
#include "caitlyn.h"
#include "caitlyn_fs.h"
#include "tamper.h"
#include "caitlyn_power.h"
#include "esp_log.h"

static const char *TAG = "caitlyn_cmd";

// ---------------------------------------------------------------------------
// Individual command handlers
// These are intentionally thin wrappers. Replace the ESP_LOGI lines with
// the real Bruce function calls (e.g. wifi_deauth_all(), ble_spam_start(), etc.)
// ---------------------------------------------------------------------------

void caitlyn_cmd_deauth_all(void)
{
    ESP_LOGI(TAG, ">>> DEAUTH ALL");
    caitlyn_fs_log_append("attacks.log", "deauth_all triggered");
    // TODO: call Bruce WiFi deauth flood
}

void caitlyn_cmd_deauth_target(void)
{
    ESP_LOGI(TAG, ">>> DEAUTH TARGET");
    caitlyn_fs_log_append("attacks.log", "deauth_target triggered");
    // TODO: call Bruce targeted deauth
}

void caitlyn_cmd_ble_spam(void)
{
    ESP_LOGI(TAG, ">>> BLE SPAM");
    caitlyn_fs_log_append("attacks.log", "ble_spam triggered");
    // TODO: call Bruce BLE spam
}

void caitlyn_cmd_spam_all(void)
{
    ESP_LOGI(TAG, ">>> SPAM ALL (iOS/Android/Windows/Samsung)");
    caitlyn_fs_log_append("attacks.log", "spam_all triggered");
    // TODO: call Bruce multi-ecosystem spam
}

void caitlyn_cmd_subghz_jam(void)
{
    ESP_LOGI(TAG, ">>> SUB-GHZ JAM");
    caitlyn_fs_log_append("attacks.log", "subghz_jam triggered");
    // TODO: call Bruce CC1101 jammer
}

void caitlyn_cmd_subghz_replay(void)
{
    ESP_LOGI(TAG, ">>> SUB-GHZ REPLAY");
    caitlyn_fs_log_append("attacks.log", "subghz_replay triggered");
    // TODO: call Bruce Sub-GHz replay
}

void caitlyn_cmd_evil_portal(void)
{
    ESP_LOGI(TAG, ">>> EVIL PORTAL");
    caitlyn_fs_log_append("attacks.log", "evil_portal triggered");
    // TODO: call Bruce EvilPortal
}

void caitlyn_cmd_badble_now(void)
{
    ESP_LOGI(TAG, ">>> BADBLE / BADUSB");
    caitlyn_fs_log_append("attacks.log", "badble_now triggered");
    // TODO: call Bruce BadBLE / BadUSB
}

void caitlyn_cmd_scan_all(void)
{
    ESP_LOGI(TAG, ">>> SCAN ALL (WiFi + BLE + Sub-GHz)");
    caitlyn_fs_log_append("attacks.log", "scan_all triggered");
    // TODO: call multi-radio scan
}

void caitlyn_cmd_stealth_mode(void)
{
    ESP_LOGI(TAG, ">>> STEALTH MODE");
    caitlyn_fs_log_append("attacks.log", "stealth_mode triggered");
    caitlyn_power_set_screen_on(false);
    // TODO: reduce TX power, stop LEDs, mute mic feedback
}

void caitlyn_cmd_wipe_logs(void)
{
    ESP_LOGI(TAG, ">>> WIPE LOGS");
    caitlyn_fs_secure_wipe(false);
}

void caitlyn_cmd_help(void)
{
    ESP_LOGI(TAG, ">>> HELP – Caitlyn status: %s", caitlyn_get_status_string());
    // TODO: speak or display available commands
}

void caitlyn_cmd_self_destruct(void)
{
    ESP_LOGW(TAG, ">>> SELF DESTRUCT");
    tamper_force_trigger();
}

void caitlyn_cmd_conference_takeover(void)
{
    ESP_LOGI(TAG, ">>> CONFERENCE TAKEOVER chain");
    // Pre-defined multi-step chain
    caitlyn_execute_command_string("deauth all and spam all then evil portal");
}

void caitlyn_cmd_lockdown(void)
{
    ESP_LOGI(TAG, ">>> LOCKDOWN chain");
    caitlyn_execute_command_string("subghz jam and deauth all then stealth mode");
}

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------

esp_err_t caitlyn_commands_register_all(void)
{
    ESP_LOGI(TAG, "Registering built-in Caitlyn commands...");

    caitlyn_register_command("deauth_all",          caitlyn_cmd_deauth_all);
    caitlyn_register_command("deauth_target",       caitlyn_cmd_deauth_target);
    caitlyn_register_command("ble_spam",            caitlyn_cmd_ble_spam);
    caitlyn_register_command("spam_all",            caitlyn_cmd_spam_all);
    caitlyn_register_command("subghz_jam",          caitlyn_cmd_subghz_jam);
    caitlyn_register_command("subghz_replay",       caitlyn_cmd_subghz_replay);
    caitlyn_register_command("evil_portal",         caitlyn_cmd_evil_portal);
    caitlyn_register_command("badble_now",          caitlyn_cmd_badble_now);
    caitlyn_register_command("scan_all",            caitlyn_cmd_scan_all);
    caitlyn_register_command("stealth_mode",        caitlyn_cmd_stealth_mode);
    caitlyn_register_command("wipe_logs",           caitlyn_cmd_wipe_logs);
    caitlyn_register_command("help",                caitlyn_cmd_help);
    caitlyn_register_command("self_destruct",       caitlyn_cmd_self_destruct);
    caitlyn_register_command("conference_takeover", caitlyn_cmd_conference_takeover);
    caitlyn_register_command("lockdown",            caitlyn_cmd_lockdown);

    ESP_LOGI(TAG, "All built-in commands registered");
    return ESP_OK;
}
