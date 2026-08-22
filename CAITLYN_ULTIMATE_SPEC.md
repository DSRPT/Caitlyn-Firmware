# Caitlyn Ultimate Spec
**Caitlyn fka Bruce — The Complete AI-Enhanced Red Team ESP32 Platform**

**Version:** Ultimate Spec v3.1  
**Target Hardware:** LILYGO T-Embed CC1101 / CC1101 Plus (ESP32-S3)  
**Base Firmware:** Bruce 1.16+ (AGPL-3.0)  
**License:** AGPL-3.0 (inherits from Bruce)  
**Repo:** https://github.com/DSRPT/Caitlyn-Firmware  

---

## 1. Vision

Caitlyn is the evolved, AI-powered successor to Bruce.  
It adds offline voice control, push-to-talk operation, tamper protection, advanced attack chaining, multi-radio firepower (including NyanBOX-style nRF24), and strong engineering foundations (power management, configuration persistence, WebUI, error recovery) while remaining fully compatible with the Bruce ecosystem.

**Personality / Branding**  
- Boot splash and occasional voice feedback reference the “Bruce → Caitlyn” transition.  
- Tone: professional, slightly sassy, highly capable red-team partner.

---

## 2. Core Feature Set (Priority Order)

### P0 — Must Have (Foundation)

| # | Feature | Description | Status |
|---|---------|-------------|--------|
| 1 | **Power Management & Battery Optimization** | Deep sleep, screen timeout, radio duty-cycling, inference only while button held, low-power I2S mic handling | Highest priority – start here |
| 2 | **NVS Configuration Persistence** | Store PTT/Wake-Word preference, voice threshold, tooltip delay, tamper sensitivity, preferred model path, etc. | Highest priority – start here |
| 3 | **Push-to-Talk (Default)** | Hold center rotary button (GPIO 0) → blue “CAITLYN LISTENING…” overlay → release to process command | Core UX |
| 4 | **Wake-Word Mode (Toggleable)** | “Hey Caitlyn” continuous listening option | Core UX |
| 5 | **TinyML Voice Engine** | Edge Impulse model (Log-Mel + PCEN preferred), 12+ commands + natural language chaining | Core AI |
| 6 | **Tamper Detection + Self-Destruct** | Acoustic anomaly (mic) + triple-press center button → screen/LEDs off + secure LittleFS wipe | Security |
| 7 | **Scrolling Tooltips** | 2-second hover on any menu item shows scrolling explanation + red-team tip | UX |
| 8 | **Attack Chaining + Macros** | Natural language (“deauth all and spam BLE then evil portal”) + voice-recordable macros | Power |
| 9 | **Full Bruce Compatibility** | All existing WiFi, BLE, Sub-GHz (CC1101), NFC (PN532), IR, BadUSB/BadBLE, WebUI, JS Interpreter, etc. | Compatibility |

### P1 — High Value (Next)

| # | Feature | Description |
|---|---------|-------------|
| 10 | **WebUI Expansion** | Model upload, live status, macro manager, log viewer, OTA |
| 11 | **File System Layout** | Clean structure on LittleFS + SD for models, macros, logs, payloads |
| 12 | **Hardware Expansion Map** | Exact free GPIOs + SPI sharing for 1–3 nRF24 modules |
| 13 | **Error Handling & Fallbacks** | Model load failure → pure button menu; button stuck recovery; watchdog |
| 14 | **Calibration Menu** | Mic gain, confidence threshold, tooltip timeout, sensitivity |
| 15 | **Audio Feedback** | Better use of MAX98357A speaker (short spoken cues / tones) |
| 16 | **Visual Live Feedback** | Spectrum / client count / signal strength overlays during attacks |
| 17 | **NyanBOX Triple nRF24 Support** | Multi-module 2.4 GHz scan / jam / MouseJacking |
| 18 | **MouseJacking Suite** | Full nRF24 HID injection capabilities |
| 19 | **AirTag / Tracker Detector** | Persistence tracking + RSSI alerts |
| 20 | **JS Interpreter Integration** | Expose Caitlyn AI functions to Bruce’s mQuickJS |

### P2 — Polish & Stretch

| # | Feature | Description |
|---|---------|-------------|
| 21 | First-boot “Authorized use only” confirmation | Legal/ethical layer |
| 22 | Multi-level secure wipe options | Logs only / everything |
| 23 | Optional tiny offline LLM | “Explain this” using llama2.c-style model (experimental) |
| 24 | Multi-language command packs | Beyond multi-voice samples |
| 25 | Maintainability strategy | Stay compatible with upstream Bruce 1.16+ |
| 26 | Automated testing plan | Concrete hardware test cases |

---

## 3. Voice Command Vocabulary (Current)

| Phrase | Activation |
|--------|------------|
| hey caitlyn | Wake / ready |
| deauth target | Targeted deauth |
| deauth all | Full deauth flood |
| ble spam | BLE spam |
| spam all | Cross-ecosystem spam |
| subghz jam | Smart Sub-GHz jam |
| subghz replay | Replay last captured signal |
| evil portal | EvilPortal + deauth |
| badble now | BadBLE / BadUSB |
| scan all | Multi-radio scan |
| stealth mode | Full stealth |
| wipe logs | Secure wipe |
| help | Status + suggestions |
| conference takeover | Pre-defined chain |
| lockdown | Multi-radio disruption chain |
| explain this | Context explanation |
| self destruct | Force tamper action |

---

## 4. Engineering Foundations (Must Be Implemented Early)

### 4.1 Power Management (Highest Priority)
- Deep sleep when idle
- Screen timeout (configurable)
- Radio TX power control + duty cycling
- Inference only while center button is held (PTT)
- Efficient I2S mic handling (no continuous sampling unless needed)
- LED and backlight management

### 4.2 NVS Configuration
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

### 4.3 File System Layout
```
/littlefs/
  /models/          ← .tflite models
  /macros/          ← recorded voice macros
  /logs/            ← attack + system logs
  /config/          ← additional config if needed
/sdcard/
  /payloads/        ← BadUSB / DuckyScript
  /captures/        ← RF / BLE captures
```

---

## 5. Hardware Notes (T-Embed CC1101)

- ESP32-S3, 16 MB Flash, 8 MB PSRAM
- Built-in SPM1423 mic + MAX98357A speaker
- Center rotary button = GPIO 0 (active low)
- CC1101 Sub-GHz + PN532 NFC already present
- Free GPIOs available for 1–3 external nRF24 modules (SPI sharing required)

---

## 6. Development Principles

- Keep full compatibility with upstream Bruce where possible
- Prefer low-power and robust error handling over flashy features
- All new code must be modular (`components/caitlyn/`, `components/tamper/`, etc.)
- Strong ethical / legal warnings in README and first-boot screen
- Document every new feature with scrolling tooltips

---

## 7. Next Immediate Steps

1. Implement **Power Management + NVS Config** (this is the current highest-impact gap).
2. Integrate Push-to-Talk + listening overlay on top of solid power foundations.
3. Add TinyML voice engine.
4. Layer on tamper detection, tooltips, chaining, and multi-radio features.

---

**Document maintained by DSRPT / Caitlyn team**  
Last updated: 2026-08-22
