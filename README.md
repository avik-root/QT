<div align="center">
  
[![QT Robot](https://readme-typing-svg.demolab.com?font=Fira+Code&weight=500&size=24&pause=1000&color=FF00FF&color2=00FFF7&color3=00FF00&color4=FFA500&color5=FF0000&center=true&vCenter=true&width=480&lines=QT+Robot+V3.0)]()
## Developed and build by [Avik Samanta](https://github.com/avik-root) and [Anusha Gupta](https://github.com/anushagupta11) | MintFire
---
### Smart OLED Display Server for ESP32-C3

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-ESP32--C3-orange.svg)](https://www.espressif.com/en/products/socs/esp32-c3)
[![Display](https://img.shields.io/badge/Display-SSD1306%200.96%22-green.svg)](https://www.solomon-systech.com/product/ssd1306/)
[![Version](https://img.shields.io/badge/Version-4.0-brightgreen.svg)](#new-releases-update)

*A multi-functional OLED display hub with touch control, alarm system, indoor/outdoor sensors, live weather, and animated screens.*

*Developed by **Avik & Anusha** — **MintFire***

</div>

---

## New Releases Update

### v4.0 — Touch Sensor + Alarm System *(Latest)*

> **Release Date:** February 2026

**New Features:**
- **🔔 Alarm System** — Set up to 3 alarms via the web interface. When triggered, the buzzer beeps continuously and the display locks to the time screen showing "ALARM!" until dismissed.
- **👆 Touch Sensor (TTP223)** — Single-tap to manually switch display screens; double-tap to dismiss an active alarm. Auto-rotation timer resets on each tap, respecting the configured interval.
- **🌐 Alarm Web Page** — New "Alarm" tab in the web dashboard with time picker, enable/disable toggles, and status indicator for each alarm slot.
- **💾 Persistent Alarms** — Alarm settings survive reboots using flash storage.
- **5-Tab Navigation** — Web UI now has Home, WiFi, API, Display, and Alarm tabs.

**Touch Sensor Behavior:**
| Action | Normal Mode | Alarm Active |
|:-------|:------------|:-------------|
| Single Tap | Switch to next screen, reset rotation timer | *Ignored* (alarm continues) |
| Double Tap | Skip a screen | **Dismiss alarm**, return to normal |

---

### v3.0 — Indoor Environment + AP Stability

- **DHT11 Indoor Sensor** — Temperature, humidity, and heat index display
- **AP WiFi Fixes** — Improved AP stability with IP-based health checks
- **Detailed Debug Logging** — `[AP-DEBUG]` serial output for WiFi troubleshooting
- **5 Display Screens** — Clock, Weather, Text Banner, Hacker Face, Indoor Temp

---

### v2.0 — Multi-Display Web Server

- **5 Auto-rotating screens** with configurable intervals
- **6 Text Animation Modes** (Static, Scroll, Bounce, Typewriter, Blink)
- **Dark-mode Web Dashboard** for all settings
- **OpenWeatherMap integration** with AQI support
- **Persistent settings** via ESP32 Preferences

---

### Upcoming Features

- 📱 **OTA Updates** — Flash new firmware over WiFi without USB
- 🎵 **Custom Alarm Tones** — Choose between different buzzer patterns
- 📊 **Temperature Logging** — Store indoor temp history with chart view on web UI
- 🌙 **Sleep Schedule** — Auto dim/off display at night with wake-on-touch
- 🔗 **MQTT Integration** — Publish sensor data to Home Assistant or similar
- ⏱️ **Stopwatch & Timer** — Countdown and stopwatch modes on display

---

## Overview

**QT Robot** transforms an ESP32-C3 Super Mini into an intelligent display station. It hosts its own WiFi network for configuration while simultaneously connecting to the internet for real-time data. Five auto-rotating display screens show time, weather, indoor environment, custom text animations, and a hacker-themed visual — all configurable through a sleek dark-mode web interface. Touch the sensor to manually switch screens, or set alarms that beep through the buzzer until dismissed.

### Key Capabilities

- **5 Display Screens** with configurable auto-rotation
- **Touch Sensor Control** — TTP223 capacitive touch for manual screen switching
- **Alarm System** — Up to 3 alarms with buzzer, settable via web interface
- **Indoor Temperature & Humidity** — DHT11 sensor with heat index calculation
- **Live Weather Data** via OpenWeatherMap API (temperature, humidity, AQI)
- **12-Hour AM/PM Clock** — real-time display synced via NTP
- **6 Text Animation Styles** for custom messages
- **Web Dashboard** with 5 tabs accessible from any device
- **Persistent Configuration** — all settings survive power cycles
- **Auto WiFi Reconnect** — automatically restores connection after drops
- **Always-On AP Mode** — configuration portal stays accessible even when connected to internet

---

## Display Screens

| Screen | Description |
|:------:|:------------|
| **Clock** | Real-time date, day of week, 12-hour HH:MM:SS AM/PM display via NTP |
| **Weather** | Temperature (°C), humidity (%), AQI index with quality label |
| **Text Banner** | User-defined text with selectable animation |
| **Hacker Face** | Binary rain animation with hooded figure overlay |
| **Indoor Temp** | DHT11 temperature, humidity, and heat index ("feels like") |

### Text Animation Modes

| Mode | Behavior |
|:-----|:---------|
| Static | Centered, no movement |
| Scroll Left | Continuous right-to-left marquee |
| Scroll Right | Continuous left-to-right marquee |
| Bounce | Oscillates between screen edges |
| Typewriter | Characters appear sequentially |
| Blink | Alternating visibility at 500ms intervals |

---

## Hardware Requirements

### Bill of Materials

| Component | Specification | Quantity |
|:----------|:-------------|:-------:|
| ESP32-C3 Super Mini | Microcontroller with WiFi | 1 |
| OLED Display | SSD1306, 0.96", 128×64, I2C | 1 |
| TTP223 Touch Sensor | Capacitive touch module | 1 |
| DHT11 Sensor | Temperature & humidity sensor | 1 |
| Passive Buzzer | 3.3V compatible buzzer module | 1 |
| Jumper Wires | Female-to-female | ~12 |
| USB-C Cable | For power and programming | 1 |

### Wiring Diagram

```
    SSD1306 OLED              ESP32-C3 Super Mini
   ┌────────────┐            ┌───────────────────┐
   │  VCC ──────┼────────────┤ 3.3V              │
   │  GND ──────┼────────────┤ GND               │
   │  SDA ──────┼────────────┤ GPIO 6            │
   │  SCL ──────┼────────────┤ GPIO 7            │
   └────────────┘            └───────────────────┘

    TTP223 Touch              ESP32-C3 Super Mini
   ┌────────────┐            ┌───────────────────┐
   │  VCC ──────┼────────────┤ 3.3V              │
   │  GND ──────┼────────────┤ GND               │
   │  SIG ──────┼────────────┤ GPIO 3            │
   └────────────┘            └───────────────────┘

    DHT11 Sensor              ESP32-C3 Super Mini
   ┌────────────┐            ┌───────────────────┐
   │  VCC ──────┼────────────┤ 3.3V              │
   │  GND ──────┼────────────┤ GND               │
   │  DATA ─────┼────────────┤ GPIO 4            │
   └────────────┘            └───────────────────┘

    Passive Buzzer            ESP32-C3 Super Mini
   ┌────────────┐            ┌───────────────────┐
   │  + (VCC) ──┼────────────┤ GPIO 2            │
   │  - (GND) ──┼────────────┤ GND               │
   └────────────┘            └───────────────────┘
```

---

## Software Setup

### Dependencies

Install via **Arduino IDE → Library Manager**:

| Library | Version | Purpose |
|:--------|:--------|:--------|
| [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306) | Latest | OLED display driver |
| [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library) | Latest | Graphics rendering |
| [ArduinoJson](https://github.com/bblanchon/ArduinoJson) | v6.x | API response parsing |
| [DHT sensor library](https://github.com/adafruit/DHT-sensor-library) | Latest | DHT11 temperature/humidity |

> **Note**: WiFi, WebServer, HTTPClient, Preferences, Wire, and time.h are built-in — no installation needed.

### Board Configuration

| Setting | Value |
|:--------|:------|
| Board | ESP32C3 Dev Module |
| USB CDC On Boot | Enabled |
| Flash Size | 4MB |
| Upload Speed | 921600 |

### Build & Upload

1. Open `qt_v4.0.ino` in Arduino IDE
2. Configure board settings as above
3. Select the correct COM/serial port
4. Click **Upload**

---

## Getting Started

### 1. First Boot

After uploading, QT Robot will:
1. Display the boot logo (v4.0) on the OLED
2. Start the WiFi access point
3. Launch the web server

### 2. Connect to QT Robot

| Setting | Value |
|:--------|:------|
| WiFi SSID | `QT-Robot` |
| Password | `12345678` |
| Web Interface | `http://192.168.4.1` |

### 3. Configure via Web Interface

Navigate to `http://192.168.4.1` and configure:

| Page | URL | What to Configure |
|:-----|:----|:-----------------|
| **Home** | `/` | Enable/disable 5 screens, rotation timing, status overview |
| **WiFi** | `/wifi` | Connect to your home network for internet access |
| **API** | `/api` | OpenWeatherMap API key, city name, refresh interval |
| **Display** | `/display` | Custom text content and animation style |
| **Alarm** | `/alarm` | Set up to 3 alarms with time picker and enable/disable |

### 4. Weather API Setup

1. Create a free account at [openweathermap.org](https://openweathermap.org/appid)
2. Navigate to **My API Keys** and copy your key
3. Open `http://192.168.4.1/api`
4. Enter your API key and city name (e.g., `Kolkata`)
5. Select refresh interval and save

> **Free tier**: 60 API calls/minute, 1,000,000 calls/month.

### 5. Setting an Alarm

1. Go to `http://192.168.4.1/alarm`
2. Set time using the time picker for any of the 3 alarm slots
3. Toggle the switch to enable
4. Click **Save Alarms**
5. When alarm fires: buzzer beeps, display shows time + "ALARM!"
6. **Double-tap** the touch sensor to dismiss

---

## Architecture

```
┌──────────────────────────────────────────────────┐
│                   QT Robot v4.0                  │
│                                                  │
│   ┌──────────┐    ┌──────────┐    ┌───────────┐  │
│   │ WiFi AP  │    │ WiFi STA │    │ Web Server│  │
│   │ (Config) │    │(Internet)│    │  (Port 80)│  │
│   └─────┬────┘    └─────┬────┘    └─────┬─────┘  │
│         │               │               │        │
│         │    ┌──────────┴──────────┐    │        │
│         │    │   NTP    │  Weather │    │        │
│         │    │  Server  │   API    │    │        │
│         │    └──────────┴──────────┘    │        │
│         │                               │        │
│         └──────────────┬────────────────┘        │
│                        │                         │
│              ┌─────────┴─────────┐               │
│              │   Display Engine  │               │
│              │  ┌─────┬────────┐ │               │
│              │  │Clock│Weather │ │               │
│              │  ├─────┼────────┤ │               │
│              │  │Text │ Hacker │ │               │
│              │  ├─────┼────────┤ │               │
│              │  │Indoor│ Alarm │ │               │
│              │  └─────┴────────┘ │               │
│              └─────────┬─────────┘               │
│              ┌─────────┴─────────┐               │
│              │     Peripherals    │              │
│              │  ┌───────┬───────┐ │              │
│              │  │TTP223 │Buzzer │ │              │
│              │  │Touch  │Alarm  │ │              │
│              │  └───────┴───────┘ │              │
│              └─────────┬─────────┘               │
│              ┌─────────┴─────────┐               │
│              │  0.96" SSD1306    │               │
│              │   OLED Display    │               │
│              └───────────────────┘               │
└──────────────────────────────────────────────────┘
```

---

## Configuration Reference

### Default Access Point

```cpp
const char* ap_ssid     = "QT-Robot";
const char* ap_password  = "12345678";
```

### Pin Mapping

```cpp
#define SDA_PIN    6   // OLED I2C data
#define SCL_PIN    7   // OLED I2C clock
#define TOUCH_PIN  3   // TTP223 touch sensor
#define DHT_PIN    4   // DHT11 data pin
#define BUZZER_PIN 2   // Passive buzzer
#define LED_PIN    8   // Onboard LED
```

### OLED I2C Address

Default: `0x3C`. If your display uses `0x3D`, modify in `setup()`:

```cpp
display.begin(SSD1306_SWITCHCAPVCC, 0x3D)
```

### Timezone

Default: IST (UTC+5:30 = 19800 seconds). To change:

```cpp
configTime(YOUR_OFFSET_IN_SECONDS, 0, "pool.ntp.org");
```

---

## Troubleshooting

| Symptom | Cause | Solution |
|:--------|:------|:---------|
| OLED blank/dark | Wiring or address mismatch | Verify SDA/SCL connections, try I2C address `0x3D` |
| QT-Robot WiFi not visible | AP not started | Wait 10s after boot, check serial monitor |
| Time shows "not synced" | No internet connection | Connect to WiFi via the WiFi settings page |
| Weather shows "N/A" | Missing API key or city | Configure in the API settings page |
| WiFi keeps disconnecting | Weak signal or wrong password | Move closer to router, re-enter credentials |
| Cannot access web interface | Not connected to QT-Robot AP | Ensure phone/laptop is on QT-Robot WiFi |
| API returns error code | Invalid key or rate limit | Verify API key, check free tier limits |
| DHT11 reading... | Sensor not connected | Check DHT11 wiring to GPIO 4, ensure VCC is 3.3V |
| Touch not responding | TTP223 not detected | Verify TTP223 SIG pin is on GPIO 3, check power |
| Indoor screen blank | DHT11 needs warm-up | Wait 2–5 seconds after boot for first reading |
| Buzzer not beeping | Wiring issue | Check buzzer + pin on GPIO 2, ensure GND connected |
| Alarm not firing | Time not synced | Connect to WiFi first so NTP can sync the clock |
| Can't dismiss alarm | Wrong gesture | Must **double-tap** (2 taps within 400ms), single tap won't work |

---

## Project Files

```
QT/
├── qt_v4.0.ino     # QT Robot v4.0 firmware (latest Beta)
├── qt_v3.0.ino     # QT Robot v3.0 firmware (Current)
├── qt_v2.0.ino     # QT Robot v3.0 firmware (previous)
├── qt_v1.0.ino     # QT Robot v1.0 firmware (legacy)
└── README.md       # Project documentation
```

---

## License

This project is licensed under the **MIT License** — free to use, modify, and distribute.

---

<div align="center">

**QT Robot v4.0** — Built with ESP32-C3 Super Mini + SSD1306 OLED + TTP223 + DHT11 + Buzzer

*Developed by Avik & Anusha — MintFire*

</div>
