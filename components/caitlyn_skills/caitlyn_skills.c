#include "caitlyn_skills.h"
#include "caitlyn.h"
#include "caitlyn_fs.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static const char *TAG = "caitlyn_skills";

// ---------------------------------------------------------------------------
// Pre-loaded skill definitions
// These are the "pre-loaded skills" that ship with Caitlyn.
// Each chain is executed by the existing command parser.
// ---------------------------------------------------------------------------

static const caitlyn_skill_t PRELOADED_SKILLS[] = {
    {
        .id          = "recon",
        .name        = "Full Recon",
        .description = "Scan WiFi + BLE + Sub-GHz and log results",
        .chain       = "scan all"
    },
    {
        .id          = "disrupt",
        .name        = "Network Disrupt",
        .description = "Deauth all networks then start BLE spam",
        .chain       = "deauth all and spam all"
    },
    {
        .id          = "conference",
        .name        = "Conference Takeover",
        .description = "Deauth + multi-spam + EvilPortal chain",
        .chain       = "deauth all and spam all then evil portal"
    },
    {
        .id          = "lockdown",
        .name        = "Area Lockdown",
        .description = "Sub-GHz jam + deauth + stealth",
        .chain       = "subghz jam and deauth all then stealth mode"
    },
    {
        .id          = "stealth",
        .name        = "Stealth Entry",
        .description = "Immediate screen off and low profile",
        .chain       = "stealth mode"
    },
    {
        .id          = "ghost",
        .name        = "Ghost Mode",
        .description = "Stealth + wipe recent logs",
        .chain       = "stealth mode and wipe logs"
    },
    {
        .id          = "portal",
        .name        = "Portal Drop",
        .description = "Deauth target then raise EvilPortal",
        .chain       = "deauth target then evil portal"
    },
    {
        .id          = "bleflood",
        .name        = "BLE Flood",
        .description = "Maximum cross-ecosystem BLE spam",
        .chain       = "spam all"
    },
    {
        .id          = "subghz",
        .name        = "Sub-GHz Burst",
        .description = "Jam then attempt replay of last capture",
        .chain       = "subghz jam then subghz replay"
    },
    {
        .id          = "selftest",
        .name        = "Self Test",
        .description = "Run help status and confirm system alive",
        .chain       = "help"
    },
    {
        .id          = "panic",
        .name        = "Panic Wipe",
        .description = "Immediate self-destruct / secure wipe",
        .chain       = "self destruct"
    },
    {
        .id          = "badusb",
        .name        = "BadUSB Now",
        .description = "Trigger BadBLE / BadUSB payload",
        .chain       = "badble now"
    }
};

static const int SKILL_COUNT = sizeof(PRELOADED_SKILLS) / sizeof(PRELOADED_SKILLS[0]);
static bool s_initialized = false;

static void str_to_lower(char *s)
{
    for (; *s; ++s) *s = (char)tolower((unsigned char)*s);
}

// Concrete runners (one per skill index)
static void run_skill_0(void)  { caitlyn_skills_run(PRELOADED_SKILLS[0].id); }
static void run_skill_1(void)  { caitlyn_skills_run(PRELOADED_SKILLS[1].id); }
static void run_skill_2(void)  { caitlyn_skills_run(PRELOADED_SKILLS[2].id); }
static void run_skill_3(void)  { caitlyn_skills_run(PRELOADED_SKILLS[3].id); }
static void run_skill_4(void)  { caitlyn_skills_run(PRELOADED_SKILLS[4].id); }
static void run_skill_5(void)  { caitlyn_skills_run(PRELOADED_SKILLS[5].id); }
static void run_skill_6(void)  { caitlyn_skills_run(PRELOADED_SKILLS[6].id); }
static void run_skill_7(void)  { caitlyn_skills_run(PRELOADED_SKILLS[7].id); }
static void run_skill_8(void)  { caitlyn_skills_run(PRELOADED_SKILLS[8].id); }
static void run_skill_9(void)  { caitlyn_skills_run(PRELOADED_SKILLS[9].id); }
static void run_skill_10(void) { caitlyn_skills_run(PRELOADED_SKILLS[10].id); }
static void run_skill_11(void) { caitlyn_skills_run(PRELOADED_SKILLS[11].id); }

static caitlyn_command_handler_t s_runners[] = {
    run_skill_0, run_skill_1, run_skill_2, run_skill_3,
    run_skill_4, run_skill_5, run_skill_6, run_skill_7,
    run_skill_8, run_skill_9, run_skill_10, run_skill_11
};

esp_err_t caitlyn_skills_init(void)
{
    if (s_initialized) return ESP_OK;

    ESP_LOGI(TAG, "Loading %d pre-loaded skills...", SKILL_COUNT);

    for (int i = 0; i < SKILL_COUNT && i < (int)(sizeof(s_runners)/sizeof(s_runners[0])); i++) {
        caitlyn_register_command(PRELOADED_SKILLS[i].id, s_runners[i]);
        ESP_LOGI(TAG, "  skill[%d] %s – %s", i, PRELOADED_SKILLS[i].id, PRELOADED_SKILLS[i].name);
    }

    s_initialized = true;
    ESP_LOGI(TAG, "Pre-loaded skills ready");
    return ESP_OK;
}

int caitlyn_skills_count(void)
{
    return SKILL_COUNT;
}

const caitlyn_skill_t *caitlyn_skills_get(int index)
{
    if (index < 0 || index >= SKILL_COUNT) return NULL;
    return &PRELOADED_SKILLS[index];
}

const caitlyn_skill_t *caitlyn_skills_find(const char *id)
{
    if (!id) return NULL;
    char buf[32];
    strncpy(buf, id, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    str_to_lower(buf);

    for (int i = 0; i < SKILL_COUNT; i++) {
        if (strcmp(PRELOADED_SKILLS[i].id, buf) == 0) {
            return &PRELOADED_SKILLS[i];
        }
    }
    return NULL;
}

esp_err_t caitlyn_skills_run(const char *id)
{
    const caitlyn_skill_t *skill = caitlyn_skills_find(id);
    if (!skill) {
        ESP_LOGW(TAG, "Unknown skill: %s", id ? id : "(null)");
        return ESP_ERR_NOT_FOUND;
    }

    ESP_LOGI(TAG, "Running skill '%s' (%s)", skill->id, skill->name);
    caitlyn_fs_log_append("skills.log", skill->id);
    caitlyn_execute_command_string(skill->chain);
    return ESP_OK;
}

esp_err_t caitlyn_skills_list(char *buf, size_t buf_len)
{
    if (!buf || buf_len < 8) return ESP_ERR_INVALID_ARG;
    buf[0] = '\0';
    size_t used = 0;
    for (int i = 0; i < SKILL_COUNT; i++) {
        size_t len = strlen(PRELOADED_SKILLS[i].id);
        if (used + len + 2 >= buf_len) break;
        if (used > 0) buf[used++] = '\n';
        memcpy(buf + used, PRELOADED_SKILLS[i].id, len);
        used += len;
        buf[used] = '\0';
    }
    return ESP_OK;
}
