#include "caitlyn.h"
#include "caitlyn_config.h"
#include "caitlyn_power.h"
#include "tamper.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static const char *TAG = "caitlyn";

#define MAX_COMMANDS 32
#define MAX_CMD_NAME 32
#define MAX_CHAIN_PARTS 8

typedef struct {
    char name[MAX_CMD_NAME];
    caitlyn_command_handler_t handler;
} command_entry_t;

static command_entry_t s_commands[MAX_COMMANDS];
static int s_command_count = 0;
static bool s_initialized = false;
static bool s_listening = false;
static bool s_ptt_was_active = false;
static char s_status[64] = "Caitlyn ready";

// Simple ring buffer placeholder for audio (real implementation would use I2S DMA)
#define AUDIO_BUFFER_SAMPLES 16000  // ~1 second at 16 kHz
static int16_t s_audio_buffer[AUDIO_BUFFER_SAMPLES];
static size_t s_audio_write_idx = 0;

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

static void set_status(const char *msg)
{
    strncpy(s_status, msg, sizeof(s_status) - 1);
    s_status[sizeof(s_status) - 1] = '\0';
    ESP_LOGI(TAG, "Status: %s", s_status);
}

static void on_tamper_triggered(void)
{
    ESP_LOGW(TAG, "Tamper action executing – going stealth + wipe");
    set_status("CAITLYN SELF-DESTRUCTED");

    // Logical screen off
    caitlyn_power_set_screen_on(false);

    // TODO: call Bruce stop_all_attacks() and LittleFS format here
    // esp_littlefs_format("/littlefs");
}

static void str_to_lower(char *s)
{
    for (; *s; ++s) {
        *s = (char)tolower((unsigned char)*s);
    }
}

static caitlyn_command_handler_t find_handler(const char *name)
{
    for (int i = 0; i < s_command_count; i++) {
        if (strcmp(s_commands[i].name, name) == 0) {
            return s_commands[i].handler;
        }
    }
    return NULL;
}

// Very simple chain splitter: splits on " and " or " then "
static void execute_chain(const char *input)
{
    char buf[256];
    strncpy(buf, input, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    str_to_lower(buf);

    char *parts[MAX_CHAIN_PARTS];
    int part_count = 0;

    char *token = strtok(buf, " ");
    char current[64] = {0};
    size_t cur_len = 0;

    while (token && part_count < MAX_CHAIN_PARTS) {
        if (strcmp(token, "and") == 0 || strcmp(token, "then") == 0) {
            if (cur_len > 0) {
                current[cur_len] = '\0';
                parts[part_count] = strdup(current);
                part_count++;
                cur_len = 0;
                current[0] = '\0';
            }
        } else {
            if (cur_len > 0 && cur_len < sizeof(current) - 2) {
                current[cur_len++] = '_';  // turn spaces into underscores for matching
            }
            size_t tlen = strlen(token);
            if (cur_len + tlen < sizeof(current) - 1) {
                memcpy(current + cur_len, token, tlen);
                cur_len += tlen;
            }
        }
        token = strtok(NULL, " ");
    }
    if (cur_len > 0 && part_count < MAX_CHAIN_PARTS) {
        current[cur_len] = '\0';
        parts[part_count] = strdup(current);
        part_count++;
    }

    for (int i = 0; i < part_count; i++) {
        ESP_LOGI(TAG, "Executing chain part: '%s'", parts[i]);
        caitlyn_command_handler_t h = find_handler(parts[i]);
        if (h) {
            h();
        } else {
            ESP_LOGW(TAG, "Unknown command in chain: %s", parts[i]);
        }
        free(parts[i]);
        vTaskDelay(pdMS_TO_TICKS(300));  // small gap between chained actions
    }
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

esp_err_t caitlyn_init(void)
{
    if (s_initialized) {
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing Caitlyn core...");

    // Config first
    ESP_ERROR_CHECK(caitlyn_config_init());

    // Power management
    ESP_ERROR_CHECK(caitlyn_power_init());

    // Tamper detection
    ESP_ERROR_CHECK(tamper_init(on_tamper_triggered));

    s_command_count = 0;
    s_listening = false;
    s_ptt_was_active = false;
    set_status("Caitlyn initialized");

    s_initialized = true;
    ESP_LOGI(TAG, "Caitlyn core ready");
    return ESP_OK;
}

esp_err_t caitlyn_start(void)
{
    if (!s_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    set_status("Caitlyn online");
    return ESP_OK;
}

void caitlyn_tick(void)
{
    if (!s_initialized) {
        return;
    }

    // Keep power management alive
    caitlyn_power_tick();

    // Track PTT transitions
    bool ptt = caitlyn_power_is_ptt_active();
    if (ptt && !s_ptt_was_active) {
        // Button just pressed
        caitlyn_on_ptt_changed(true);
    } else if (!ptt && s_ptt_was_active) {
        // Button just released
        caitlyn_on_ptt_changed(false);
    }
    s_ptt_was_active = ptt;
}

void caitlyn_on_ptt_changed(bool pressed)
{
    if (caitlyn_config_get_voice_mode() != CAITLYN_VOICE_MODE_PTT) {
        return;  // only relevant in PTT mode
    }

    if (pressed) {
        s_listening = true;
        s_audio_write_idx = 0;
        set_status("CAITLYN LISTENING...");
        caitlyn_power_reset_activity();
        ESP_LOGI(TAG, "PTT pressed – listening started");
    } else {
        s_listening = false;
        set_status("Processing...");
        ESP_LOGI(TAG, "PTT released – processing audio");
        caitlyn_process_audio_buffer();
    }
}

void caitlyn_process_audio_buffer(void)
{
    // Placeholder for real TinyML inference.
    // In a full implementation this would:
    //  1. Run the Edge Impulse / TFLite model on s_audio_buffer
    //  2. Get the top class + confidence
    //  3. If confidence > threshold, call caitlyn_execute_command_string()

    // For now we just log that processing would happen here.
    ESP_LOGI(TAG, "Audio buffer processing stub (samples=%u)", (unsigned)s_audio_write_idx);
    set_status("Caitlyn ready");

    // Example of how a real result would be handled:
    // if (confidence >= caitlyn_config_get_confidence()) {
    //     caitlyn_execute_command_string(detected_label);
    // }
}

bool caitlyn_is_listening(void)
{
    return s_listening;
}

esp_err_t caitlyn_register_command(const char *name, caitlyn_command_handler_t handler)
{
    if (s_command_count >= MAX_COMMANDS) {
        return ESP_ERR_NO_MEM;
    }
    if (!name || !handler) {
        return ESP_ERR_INVALID_ARG;
    }

    strncpy(s_commands[s_command_count].name, name, MAX_CMD_NAME - 1);
    s_commands[s_command_count].name[MAX_CMD_NAME - 1] = '\0';
    s_commands[s_command_count].handler = handler;
    s_command_count++;

    ESP_LOGI(TAG, "Registered command: %s", name);
    return ESP_OK;
}

void caitlyn_execute_command_string(const char *cmd)
{
    if (!cmd || !*cmd) {
        return;
    }
    ESP_LOGI(TAG, "Executing: %s", cmd);
    execute_chain(cmd);
}

const char *caitlyn_get_status_string(void)
{
    return s_status;
}
