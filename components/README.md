# Caitlyn Components

Modular building blocks for the Caitlyn firmware (AI-enhanced successor to Bruce).

## Directory Structure

```
components/
├── caitlyn_config/       # NVS-backed configuration (voice mode, thresholds, timeouts)
├── caitlyn_power/        # Power management, screen timeout, PTT button tracking, deep sleep
├── tamper/               # Acoustic + triple-press tamper detection + self-destruct
├── caitlyn/              # Core voice engine, PTT state machine, command chaining
├── caitlyn_ui/           # Listening overlay + scrolling tooltip helpers
├── caitlyn_fs/           # Standard directory layout + secure wipe helpers
├── caitlyn_commands/     # Built-in command handlers (deauth, BLE spam, chains, etc.)
├── README.md
└── INTEGRATION_EXAMPLE.md
```

## Quick Start

```c
#include "caitlyn.h"
#include "caitlyn_ui.h"
#include "caitlyn_fs.h"
#include "caitlyn_commands.h"

void app_main(void)
{
    // ... Bruce hardware + LVGL init ...

    caitlyn_fs_init();
    caitlyn_init();                 // config → power → tamper
    caitlyn_ui_init();
    caitlyn_commands_register_all();
    caitlyn_start();

    while (1) {
        caitlyn_tick();
        caitlyn_ui_tooltip_tick();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
```

See `INTEGRATION_EXAMPLE.md` for the full recommended integration pattern.

## Key Public APIs

| Function | Purpose |
|----------|---------|
| `caitlyn_power_is_ptt_active()` | True while center button is held |
| `caitlyn_is_listening()` | True while listening overlay should be shown |
| `caitlyn_execute_command_string("deauth all and spam BLE")` | Natural language chaining |
| `tamper_force_trigger()` | Force self-destruct (voice command) |
| `caitlyn_config_save()` | Persist settings to NVS |
| `caitlyn_fs_secure_wipe(false)` | Wipe logs + macros |
| `caitlyn_commands_register_all()` | Register all built-in attack commands |

## Status

- **caitlyn_config** – fully functional
- **caitlyn_power** – fully functional (PTT + timeout + deep sleep)
- **tamper** – fully functional
- **caitlyn** – functional state machine + chaining parser (TinyML inference is a clear integration point)
- **caitlyn_ui** – functional timer/overlay logic (LVGL object creation is a clear integration point)
- **caitlyn_fs** – fully functional
- **caitlyn_commands** – complete set of handlers with logging (real Bruce attack calls are TODOs)

## Next Steps for Full Integration

1. Replace the `ESP_LOGI` placeholders in `caitlyn_commands.c` with real Bruce attack function calls.
2. Wire the listening overlay to actual LVGL objects.
3. Feed real mic volume samples into `tamper_check_mic()`.
4. Connect the Edge Impulse / TFLite model inside `caitlyn_process_audio_buffer()`.
