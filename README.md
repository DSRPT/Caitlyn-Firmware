# Caitlyn Firmware

**Caitlyn fka Bruce** — The AI-Enhanced Red Team ESP32 Platform

Caitlyn is the evolved successor to Bruce firmware for the LILYGO T-Embed CC1101 (and compatible boards). It adds offline voice control, push-to-talk operation, tamper protection, advanced attack chaining, multi-radio capabilities, and strong engineering foundations while remaining compatible with the Bruce ecosystem.

**Current Status:** Foundation phase  
**Target:** Ultimate field-ready red-team tool  
**License:** AGPL-3.0 (inherits from Bruce)

---

## Key Documents

| Document | Description |
|----------|-------------|
| [CAITLYN_ULTIMATE_SPEC.md](CAITLYN_ULTIMATE_SPEC.md) | Complete feature specification + priority order |
| [CLAUDE_METAPROMPT_ULTIMATE.md](CLAUDE_METAPROMPT_ULTIMATE.md) | Full-spectrum metaprompt to give Claude for building the entire project |
| [PHASE1_POWER_NVS_GUIDE.md](PHASE1_POWER_NVS_GUIDE.md) | Immediate next step: Power Management + NVS Config |

---

## Vision

Caitlyn transforms the T-Embed CC1101 into a voice-controlled, tamper-aware, multi-radio red-team implant with:

- Offline TinyML voice commands + natural language chaining
- Push-to-Talk (default) and Wake-Word modes
- Tamper detection with secure wipe
- Scrolling tooltips and strong UX
- NyanBOX-style 2.4 GHz firepower (nRF24)
- Full Bruce compatibility
- Solid power management and configuration persistence

---

## Immediate Priority

**Phase 1 – Power Management + NVS Configuration** is the highest-impact remaining gap.  
Start there before adding voice overlays or multi-radio features. See `PHASE1_POWER_NVS_GUIDE.md`.

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

## Development

Base: Bruce 1.16+  
Approach: Modular components under `components/`  
Style: Clean C APIs, FreeRTOS, LVGL, low-power first

---

**Maintained by DSRPT**  
Repo: https://github.com/DSRPT/Caitlyn-Firmware
