# Caitlyn Firmware

**Caitlyn fka Bruce** — The AI-Enhanced Red Team ESP32 Platform

Caitlyn is the evolved successor to Bruce firmware for the LILYGO T-Embed CC1101 (and compatible boards). It adds offline voice control, push-to-talk operation, tamper protection, advanced attack chaining, multi-radio capabilities, and strong engineering foundations while remaining compatible with the Bruce ecosystem.

**Current Status:** Audited + 12 pre-loaded skills ready  
**Target:** Ultimate field-ready red-team tool  
**License:** AGPL-3.0 (inherits from Bruce)

---

## Repository Contents

| Path | Description |
|------|-------------|
| [AUDIT.md](AUDIT.md) | Structural + completeness audit report |
| [CAITLYN_ULTIMATE_SPEC.md](CAITLYN_ULTIMATE_SPEC.md) | Complete feature specification |
| [CLAUDE_METAPROMPT_ULTIMATE.md](CLAUDE_METAPROMPT_ULTIMATE.md) | Full-spectrum Claude metaprompt |
| [components/](components/) | **Complete modular source code** |

### Components

```
components/
├── caitlyn_config/     NVS configuration
├── caitlyn_power/      Power management, PTT, deep sleep
├── tamper/             Acoustic + triple-press tamper
├── caitlyn/            Core PTT state machine + chaining
├── caitlyn_ui/         Listening overlay + tooltips
├── caitlyn_fs/         Directory layout + secure wipe
├── caitlyn_commands/   Atomic command handlers
├── caitlyn_skills/     12 pre-loaded multi-step playbooks
├── caitlyn_ota/        Model download + firmware OTA
├── caitlyn_nrf24/      NyanBOX-style multi-nRF24 skeleton
├── caitlyn_bootstrap.c Single entry-point
├── README.md
└── INTEGRATION_EXAMPLE.md
```

---

## Pre-loaded Skills (voice-triggerable)

| Voice ID | Playbook |
|----------|----------|
| `recon` | Full radio scan |
| `disrupt` | Deauth all + BLE spam |
| `conference` | Deauth + spam + EvilPortal |
| `lockdown` | Sub-GHz jam + deauth + stealth |
| `stealth` | Immediate low-profile mode |
| `ghost` | Stealth + wipe logs |
| `portal` | Targeted deauth + EvilPortal |
| `bleflood` | Max BLE spam |
| `subghz` | Jam then replay |
| `selftest` | Status check |
| `panic` | Secure self-destruct |
| `badusb` | BadBLE / BadUSB |

Hold the center button and say any of the IDs above.

---

## Quick Integration

```c
caitlyn_bootstrap();
caitlyn_start_background_task();
```

See `components/INTEGRATION_EXAMPLE.md` and `AUDIT.md` for full details.

---

## Hardware

- **Primary:** LILYGO T-Embed CC1101 / CC1101 Plus
- ESP32-S3, 16 MB Flash, 8 MB PSRAM
- Built-in mic + speaker, CC1101, PN532
- Rotary encoder center button = GPIO 0
- Optional: 1–3 external nRF24L01+ modules

---

## Legal Notice

This firmware is intended **exclusively for authorized penetration testing, red-team exercises, and research on systems you own or have explicit written permission to test**.

Unauthorized use may violate local laws.

---

**Maintained by DSRPT**  
https://github.com/DSRPT/Caitlyn-Firmware
