<div align="center">
  
[![QT Robot](https://readme-typing-svg.demolab.com?font=Fira+Code&weight=500&size=24&pause=1000&color=FF00FF&color2=00FFF7&color3=00FF00&color4=FFA500&color5=FF0000&center=true&vCenter=true&width=480&lines=QT+Robot+V1.0)]()
## Developed and build by [Avik Samanta](https://github.com/avik-root) and [Anusha Gupta](https://github.com/anushagupta11) | MintFire
---
### Smart OLED Display Server for ESP32-C3

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-ESP32--C3-orange.svg)](https://www.espressif.com/en/products/socs/esp32-c3)
[![Display](https://img.shields.io/badge/Display-SSD1306%200.96%22-green.svg)](https://www.solomon-systech.com/product/ssd1306/)

*A multi-functional OLED display hub with web-based control, live weather, and animated screens.*

</div>

---

## Overview

**QT Robot** transforms an ESP32-C3 Super Mini into an intelligent display station. It hosts its own WiFi network for configuration while simultaneously connecting to the internet for real-time data. Four auto-rotating display screens show time, weather, custom text animations, and a hacker-themed visual — all configurable through a sleek dark-mode web interface.

### Key Capabilities

- **4 Display Screens** with configurable auto-rotation
- **Live Weather Data** via OpenWeatherMap API (temperature, humidity, AQI)
- **6 Text Animation Styles** for custom messages
- **Web Dashboard** accessible from any device on the network
- **Persistent Configuration** — settings survive power cycles
- **Auto WiFi Reconnect** — automatically restores connection after drops
- **Always-On AP Mode** — configuration portal stays accessible even when connected to internet

---

## Display Screens

| Screen | Description |
|:------:|:------------|
| **Clock** | Real-time date, day of week, and large HH:MM:SS display via NTP |
| **Weather** | Temperature (°C), humidity (%), AQI index with quality label |
| **Text Banner** | User-defined text with selectable animation |
| **Hacker Face** | Binary rain animation with hooded figure overlay |

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
| Jumper Wires | Female-to-female | 4 |
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

> **Note**: WiFi, WebServer, HTTPClient, Preferences, Wire, and time.h are built-in — no installation needed.

### Board Configuration

| Setting | Value |
|:--------|:------|
| Board | ESP32C3 Dev Module |
| USB CDC On Boot | Enabled |
| Flash Size | 4MB |
| Upload Speed | 921600 |

### Build & Upload

1. Open `oled_display_server.ino` in Arduino IDE
2. Configure board settings as above
3. Select the correct COM/serial port
4. Click **Upload**

---

## Getting Started

### 1. First Boot

After uploading, QT Robot will:
1. Display the boot logo on the OLED
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
| **Home** | `/` | Enable/disable screens, rotation timing (5s–60s) |
| **WiFi** | `/wifi` | Connect to your home network for internet access |
| **API** | `/api` | OpenWeatherMap API key, city name, refresh interval |
| **Display** | `/display` | Custom text content and animation style |

### 4. Weather API Setup

1. Create a free account at [openweathermap.org](https://openweathermap.org/appid)
2. Navigate to **My API Keys** and copy your key
3. Open `http://192.168.4.1/api`
4. Enter your API key and city name (e.g., `Kolkata`)
5. Select refresh interval and save

> **Free tier**: 60 API calls/minute, 1,000,000 calls/month.

---

## Architecture

```
┌──────────────────────────────────────────────────┐
│                   QT Robot                       │
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
│              │  └─────┴────────┘ │               │
│              └─────────┬─────────┘               │
│                        │                         │
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

### I2C Pin Mapping

```cpp
#define SDA_PIN 6
#define SCL_PIN 7
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

---

## Project Files

```
Embedded System/
├── oled_display_server.ino   # QT Robot firmware
├── blink_uno.ino             # Basic WiFi AP test firmware
└── README.md                 # Project documentation
```

---

## License

This project is licensed under the **MIT License** — free to use, modify, and distribute.

---

<div align="center">

**QT Robot** — Built with ESP32-C3 Super Mini + SSD1306 OLED

</div>
