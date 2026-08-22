# Caitlyn Firmware

**Caitlyn fka Bruce** — The AI-Enhanced Red Team ESP32 Platform

Caitlyn is the evolved successor to Bruce firmware for the LILYGO T-Embed CC1101 (and compatible boards). It adds offline voice control, push-to-talk operation, tamper protection, advanced attack chaining, multi-radio capabilities, and strong engineering foundations while remaining compatible with the Bruce ecosystem.

**Current Status:** Full foundation + OTA + nRF24 skeleton ready for integration  
**Target:** Ultimate field-ready red-team tool  
**License:** AGPL-3.0 (inherits from Bruce)

---

## Repository Contents

| Path | Description |
|------|-------------|
| [CAITLYN_ULTIMATE_SPEC.md](CAITLYN_ULTIMATE_SPEC.md) | Complete feature specification + priority order |
| [CLAUDE_METAPROMPT_ULTIMATE.md](CLAUDE_METAPROMPT_ULTIMATE.md) | Full-spectrum metaprompt for Claude |
| [PHASE1_POWER_NVS_GUIDE.md](PHASE1_POWER_NVS_GUIDE.md) | Original Phase 1 guide |
| [components/](components/) | **Complete modular source code** |

### Components (ready to use)

```
components/
├── caitlyn_config/     NVS configuration
├── caitlyn_power/      Power management, PTT, deep sleep
├── tamper/             Acoustic + triple-press tamper
├── caitlyn/            Core PTT state machine + chaining
├── caitlyn_ui/         Listening overlay + tooltips
├── caitlyn_fs/         Directory layout + secure wipe
├── caitlyn_commands/   15+ built-in command handlers
├── caitlyn_ota/        Model download + firmware OTA
├── caitlyn_nrf24/      NyanBOX-style multi-nRF24 skeleton
├── caitlyn_bootstrap.c Single entry-point example
├── README.md
└── INTEGRATION_EXAMPLE.md
```

---

## Quick Integration

```c
caitlyn_bootstrap();
caitlyn_start_background_task();
```

Or call the ticks from your existing main loop.  
See `components/INTEGRATION_EXAMPLE.md` and `components/caitlyn_bootstrap.c`.

---

## Hardware

- **Primary:** LILYGO T-Embed CC1101 / CC1101 Plus
- ESP32-S3, 16 MB Flash, 8 MB PSRAM
- Built-in mic + speaker, CC1101 Sub-GHz, PN532 NFC
- Rotary encoder with center button (GPIO 0)
- Optional: 1–3 external nRF24L01+ modules for 2.4 GHz expansion

---

## Legal Notice

This firmware is intended **exclusively for authorized penetration testing, red-team exercises, and research on systems you own or have explicit written permission to test**.

Unauthorized use may violate local laws. Always follow the law and ethical guidelines.

---

**Maintained by DSRPT**  
Repo: https://github.com/DSRPT/Caitlyn-Firmware
