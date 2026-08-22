# Caitlyn Components

This folder contains the modular building blocks for Caitlyn firmware.

## Structure

```
components/
├── caitlyn_config/     # NVS-backed configuration (voice mode, thresholds, timeouts)
├── caitlyn_power/      # Power management, screen timeout, PTT button tracking, deep sleep
├── tamper/             # Acoustic + triple-press tamper detection + self-destruct
├── caitlyn/            # Core voice engine, PTT state machine, command chaining
└── caitlyn_ui/         # Listening overlay + scrolling tooltips helpers
```

## Initialization Order

```c
#include "caitlyn.h"
#include "caitlyn_ui.h"

void app_main(void)
{
    // ... Bruce core init ...

    caitlyn_init();          // initializes config → power → tamper
    caitlyn_ui_init();
    caitlyn_start();

    // Register your attack commands
    caitlyn_register_command("deauth_all", my_deauth_all_handler);
    caitlyn_register_command("ble_spam", my_ble_spam_handler);
    // ...

    while (1) {
        caitlyn_tick();
        caitlyn_ui_tooltip_tick();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
```

## Key APIs

- `caitlyn_power_is_ptt_active()` – true while center button is held
- `caitlyn_is_listening()` – true while listening overlay should be shown
- `caitlyn_execute_command_string("deauth all and spam BLE")` – natural language chaining
- `tamper_force_trigger()` – for the "self destruct" voice command
- `caitlyn_config_save()` – persist any setting changes

## Notes

- All modules are designed to compile against ESP-IDF / Bruce-style component system.
- Real TinyML inference, LVGL object creation, and LittleFS wipe are left as clear integration points so they can be filled with the exact Bruce APIs you are using.
- Power and config are fully functional and can be used immediately.
