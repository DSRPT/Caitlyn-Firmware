#pragma once

#include "esp_err.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Caitlyn Pre-loaded Skills
 *
 * Skills are multi-step, pre-configured playbooks that can be triggered
 * by a single voice command or menu selection.
 *
 * Each skill is a named sequence of commands that the core chaining
 * engine already understands.
 */

typedef struct {
    const char *id;           // short id used in voice / menu (e.g. "recon")
    const char *name;         // human-readable name
    const char *description;  // one-line description for tooltips
    const char *chain;        // command string passed to caitlyn_execute_command_string()
} caitlyn_skill_t;

/**
 * @brief Initialize the skills subsystem and register all pre-loaded skills
 * as commands so they can be triggered by voice.
 */
esp_err_t caitlyn_skills_init(void);

/**
 * @brief Return the number of pre-loaded skills
 */
int caitlyn_skills_count(void);

/**
 * @brief Get a skill by index (0 .. count-1)
 */
const caitlyn_skill_t *caitlyn_skills_get(int index);

/**
 * @brief Find a skill by id (case-insensitive)
 */
const caitlyn_skill_t *caitlyn_skills_find(const char *id);

/**
 * @brief Execute a skill by id
 */
esp_err_t caitlyn_skills_run(const char *id);

/**
 * @brief List all skill ids into a buffer (newline separated)
 */
esp_err_t caitlyn_skills_list(char *buf, size_t buf_len);

#ifdef __cplusplus
}
#endif
