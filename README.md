# Caitlyn Firmware

**Caitlyn fka Bruce** — The AI-Enhanced Red Team ESP32 Platform

Caitlyn is the evolved successor to Bruce firmware for the LILYGO T-Embed CC1101 (and compatible boards). It adds offline voice control, push-to-talk operation, tamper protection, advanced attack chaining, multi-radio capabilities, and strong engineering foundations while remaining compatible with the Bruce ecosystem.

**Current Status:** Foundation modules complete and ready for integration  
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
├── caitlyn_config/     NVS configuration (voice mode, thresholds, timeouts)
├── caitlyn_power/      Power management, PTT tracking, deep sleep, screen timeout
├── tamper/             Acoustic + triple-press tamper detection
├── caitlyn/            Core PTT state machine + command chaining parser
├── caitlyn_ui/         Listening overlay + scrolling tooltip helpers
├── caitlyn_fs/         Directory layout + secure wipe helpers
├── caitlyn_commands/   Built-in command handlers (15+ commands + chains)
├── README.md
└── INTEGRATION_EXAMPLE.md
```

---

## Quick Integration

```c
caitlyn_fs_init();
caitlyn_init();                 // config → power → tamper
caitlyn_ui_init();
caitlyn_commands_register_all();
caitlyn_start();

// In main loop:
caitlyn_tick();
caitlyn_ui_tooltip_tick();
```

See `components/INTEGRATION_EXAMPLE.md` for the full pattern.

---

## Hardware

- **Primary:** LILYGO T-Embed CC1101 / CC1101 Plus
- ESP32-S3, 16 MB Flash, 8 MB PSRAM
- Built-in mic + speaker, CC1101 Sub-GHz, PN532 NFC
- Rotary encoder with center button (GPIO 0)

---

## Legal Notice

This firmware is intended **exclusively for authorized penetration testing, red-team exercises, and research on systems you own or have explicit written permission to test**.

Unauthorized use may violate local laws. Always follow the law and ethical guidelines.

---

**Maintained by DSRPT**  
Repo: https://github.com/DSRPT/Caitlyn-Firmware
