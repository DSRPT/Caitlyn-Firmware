# Caitlyn Integration Example

This document shows the recommended way to wire Caitlyn into a Bruce-based firmware.

## 1. Component Dependencies

In your main `CMakeLists.txt` or the board component, make sure these are available:

```
REQUIRES caitlyn caitlyn_config caitlyn_power tamper caitlyn_ui caitlyn_fs caitlyn_commands
```

## 2. Boot Sequence (recommended)

```c
#include "caitlyn.h"
#include "caitlyn_ui.h"
#include "caitlyn_fs.h"
#include "caitlyn_commands.h"
#include "caitlyn_power.h"

void caitlyn_bootstrap(void)
{
    // 1. File system layout
    caitlyn_fs_init();

    // 2. Core (config → power → tamper)
    caitlyn_init();

    // 3. UI helpers
    caitlyn_ui_init();

    // 4. Register all attack commands
    caitlyn_commands_register_all();

    // 5. Start
    caitlyn_start();

    ESP_LOGI("main", "Caitlyn bootstrap complete – status: %s", caitlyn_get_status_string());
}
```

Call `caitlyn_bootstrap()` after Bruce has finished its own hardware and LVGL init.

## 3. Main Loop / Tick

In the existing Bruce main loop (or a dedicated FreeRTOS task):

```c
while (true) {
    caitlyn_tick();               // power + PTT state machine
    caitlyn_ui_tooltip_tick();    // 2-second tooltip timer

    // Optional: feed mic volume to tamper
    // float vol = get_current_mic_volume();
    // tamper_check_mic(vol);

    vTaskDelay(pdMS_TO_TICKS(50));
}
```

## 4. PTT + Listening Overlay

When the center button is held, `caitlyn_is_listening()` becomes true.
In your LVGL UI code you can react:

```c
if (caitlyn_is_listening()) {
    caitlyn_ui_set_listening_overlay(true);
    caitlyn_ui_set_status_text(caitlyn_get_status_string());
} else {
    caitlyn_ui_set_listening_overlay(false);
}
```

## 5. Connecting Real Bruce Attacks

Open `components/caitlyn_commands/caitlyn_commands.c` and replace the `ESP_LOGI` placeholders with the actual Bruce function calls, for example:

```c
void caitlyn_cmd_deauth_all(void)
{
    // Existing Bruce API (names vary by version)
    wifi_attack_deauth_all();
    caitlyn_fs_log_append("attacks.log", "deauth_all");
}
```

## 6. First-boot Legal Screen (recommended)

```c
if (caitlyn_config_is_first_boot()) {
    // Show "Authorized use only" screen
    // Wait for user confirmation
    caitlyn_config_set_first_boot_done();
    caitlyn_config_save();
}
```

## 7. Building

After copying the `components/` folder into a Bruce tree:

```bash
idf.py set-target esp32s3
idf.py build
idf.py flash monitor
```

The components are written to be compatible with ESP-IDF component manager / Bruce's existing structure.
