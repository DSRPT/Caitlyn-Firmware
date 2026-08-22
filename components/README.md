# Caitlyn Components

Modular building blocks for the Caitlyn firmware (AI-enhanced successor to Bruce).

## Directory Structure

```
components/
├── caitlyn_config/       # NVS-backed configuration
├── caitlyn_power/        # Power management, PTT tracking, deep sleep
├── tamper/               # Acoustic + triple-press tamper detection
├── caitlyn/              # Core PTT state machine + command chaining
├── caitlyn_ui/           # Listening overlay + scrolling tooltips
├── caitlyn_fs/           # Directory layout + secure wipe
├── caitlyn_commands/     # Built-in attack command handlers
├── caitlyn_ota/          # Model download + firmware OTA helpers
├── caitlyn_nrf24/        # NyanBOX-style multi-nRF24 skeleton
├── caitlyn_bootstrap.c   # Single entry-point example
├── README.md
└── INTEGRATION_EXAMPLE.md
```

## Quick Start

```c
// After Bruce hardware + LVGL init:
caitlyn_bootstrap();
caitlyn_start_background_task();   // or call ticks from existing loop
```

## Component Status

| Component | Status | Notes |
|-----------|--------|-------|
| caitlyn_config | Complete | Full NVS load/save |
| caitlyn_power | Complete | PTT, timeout, deep sleep |
| tamper | Complete | Mic + triple-press |
| caitlyn | Complete | State machine + chaining |
| caitlyn_ui | Complete | Overlay + tooltip timers |
| caitlyn_fs | Complete | Layout + secure wipe |
| caitlyn_commands | Complete | 15 handlers + chains |
| caitlyn_ota | Complete | Model download + firmware OTA |
| caitlyn_nrf24 | Skeleton | Ready for hardware attach |

## Integration Points Still Open

1. Replace `ESP_LOGI` placeholders in `caitlyn_commands.c` with real Bruce attack calls.
2. Create actual LVGL objects inside `caitlyn_ui_set_listening_overlay()`.
3. Feed real mic volume into `tamper_check_mic()`.
4. Load the Edge Impulse / TFLite model inside `caitlyn_process_audio_buffer()`.
5. Probe real nRF24 modules and implement scan/jam/MouseJack.
