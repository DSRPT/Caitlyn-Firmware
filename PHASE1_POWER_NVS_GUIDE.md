# Phase 1 — Power Management + NVS Configuration
**Caitlyn Ultimate – Highest Impact Gap**

This is the recommended first engineering step before adding voice, PTT overlays, or multi-radio features.

---

## Goals

1. Create a robust power management subsystem.
2. Persist all user preferences in NVS.
3. Provide clean APIs for the rest of the firmware.

---

## Recommended Component Structure

```
components/
  caitlyn_power/
    CMakeLists.txt
    include/caitlyn_power.h
    caitlyn_power.c
  caitlyn_config/
    CMakeLists.txt
    include/caitlyn_config.h
    caitlyn_config.c
```

---

## Key Responsibilities

### caitlyn_power
- Screen timeout management
- Deep sleep entry / wake sources (button, timer)
- Radio duty-cycle helpers
- PTT-aware inference gating (inference only while button held)
- LED / backlight control
- Low-battery monitoring hooks

### caitlyn_config (NVS)
Store and restore:
- `voice_mode` (0 = PTT, 1 = Wake-Word)
- `confidence_threshold` (float, e.g. 0.85)
- `tooltip_delay_ms` (default 2000)
- `tamper_sensitivity`
- `preferred_model_path`
- `screen_timeout_s`
- `tx_power_level`
- Any future preferences

Use ESP-IDF NVS with a dedicated namespace (`caitlyn`).

---

## Suggested APIs

```c
// Power
void caitlyn_power_init(void);
void caitlyn_power_tick(void);               // call from main loop or timer
void caitlyn_power_request_sleep(void);
bool caitlyn_power_is_ptt_active(void);      // true while center button held
void caitlyn_power_set_screen_timeout(uint32_t seconds);
void caitlyn_power_reset_activity(void);     // call on any user interaction

// Config
void caitlyn_config_init(void);
esp_err_t caitlyn_config_load(void);
esp_err_t caitlyn_config_save(void);
int  caitlyn_config_get_voice_mode(void);
void caitlyn_config_set_voice_mode(int mode);
float caitlyn_config_get_confidence(void);
void caitlyn_config_set_confidence(float v);
// ... similar getters/setters for other fields
```

---

## Implementation Notes

- Use FreeRTOS timers for screen timeout.
- Make the center button (GPIO 0) a wake source for deep sleep.
- Keep power management independent of the TinyML engine so the rest of the system can still run if the model fails to load.
- Log power events at INFO level for debugging.
- Default to PTT mode for maximum battery life.

---

## Success Criteria for Phase 1

- [ ] Preferences survive reboot.
- [ ] Screen turns off after configured timeout and wakes on button press.
- [ ] Device can enter and leave deep sleep cleanly.
- [ ] Clean public API exists for voice mode and thresholds.
- [ ] No impact on existing Bruce functionality.

---

## Next After Phase 1

Once this foundation is solid, implement:
1. Push-to-Talk listening overlay (using `caitlyn_power_is_ptt_active()`)
2. TinyML inference gated by PTT
3. Tamper detection
4. Scrolling tooltips
5. The rest of the feature set

This order prevents power and configuration bugs from appearing later in the more complex features.
