# 🏠 Smart Hostel Energy Saver System (SHES)

> Automatic room power cutoff using PIR heat detection — saves energy when no occupant is present.

**Author:** THAROON M K 
**Platform:** ESP32 DevKit C V4  
**Simulated on:** [Wokwi](https://wokwi.com/projects/456377440609481729)  
**IDE:** VS Code + PlatformIO + Wokwi Extension

---

## 📋 Table of Contents

- [Overview](#overview)
- [How It Works](#how-it-works)
- [Components](#components)
- [Project Structure](#project-structure)
- [Pin Configuration](#pin-configuration)
- [Setup & Installation](#setup--installation)
- [Running the Simulation](#running-the-simulation)
- [WiFi Dashboard](#wifi-dashboard)
- [API Reference](#api-reference)
- [Cost Estimate](#cost-estimate)
- [Future Improvements](#future-improvements)

---

## Overview

Students in hostels routinely leave lights and fans running when they step out for lectures, canteen, or sports. This system automatically cuts power to room appliances when **no human heat signature is detected** for 5 continuous minutes.

### Key Features

- 🌡️ **PIR heat detection** — detects infrared radiation emitted by human bodies
- ⚡ **Relay auto switch** — physically cuts power to lights and fans
- ⏱️ **5-minute timeout** — prevents false cutoffs when student is sitting still
- 🌙 **LDR ambient check** — only activates lights when room is dark enough
- 🔘 **Manual override** — 30-minute override via physical push button
- 📶 **WiFi dashboard** — live web UI to monitor and control the system remotely

---

## How It Works

```
Human in room
      ↓
PIR sensor detects body heat (IR radiation)
      ↓
ESP32 reads HIGH signal on GPIO 13
      ↓
LDR checks if room is dark (ADC > 2000)
      ↓
Relay switches ON → Light / Fan activates
      ↓
No motion for 5 continuous minutes?
      ↓
Relay switches OFF → Power cut automatically
```

---

## Components

| Component | Model | Qty | Cost (INR) |
|---|---|---|---|
| Microcontroller | ESP32 DevKit C V4 | 1 | ₹350 |
| PIR Sensor | HC-SR501 | 1 | ₹65 |
| Relay Module | 5V Single Channel | 1 | ₹55 |
| LDR Sensor | Photoresistor | 1 | ₹15 |
| Push Button | Tactile Switch | 1 | ₹5 |
| Resistor | 220Ω | 2 | ₹4 |
| Resistor | 10kΩ (pull-down) | 1 | ₹2 |
| LED | Red indicator | 2 | ₹10 |
| Breadboard | 830 point | 1 | ₹70 |
| Jumper Wires | Pack | 1 | ₹50 |
| USB Cable | Power | 1 | ₹80 |
| **Total** | | | **₹706** |

---

## Project Structure

```
SHES/
├── .pio/                  ← PlatformIO build output (auto-generated)
├── .vscode/               ← VS Code settings (auto-generated)
├── include/               ← Header files (optional)
├── lib/                   ← External libraries (optional)
├── src/
│   └── main.cpp           ← ESP32 firmware — sensor logic + WiFi API
├── test/
│   └── README             ← PlatformIO test placeholder
├── .gitignore
├── dashboard.html         ← Standalone frontend dashboard
├── diagram.json           ← Wokwi circuit wiring
├── platformio.ini         ← Board + framework configuration
├── wokwi.toml             ← Wokwi simulator + port forwarding config
└── README.md              ← This file
```

---

## Pin Configuration

| GPIO | Component | Direction | Notes |
|---|---|---|---|
| 13 | PIR HC-SR501 OUT | INPUT | HIGH = heat detected |
| 12 | Relay IN | OUTPUT | HIGH = relay ON |
| 34 | LDR Analog OUT | INPUT (ADC) | High ADC value = dark room |
| 35 | Push Button | INPUT_PULLDOWN | HIGH = pressed |

### Circuit Wiring

```
ESP32 GPIO 13  ←──  PIR OUT
ESP32 GPIO 12  ──→  Relay IN
ESP32 GPIO 34  ←──  LDR AO
ESP32 GPIO 35  ←──  Push Button
ESP32 5V       ──→  PIR VCC + Relay VCC
ESP32 3V3      ──→  LDR VCC
ESP32 GND      ──→  All GND connections
```

> Full diagram available in `diagram.json` — open in [Wokwi](https://wokwi.com)

---

## Setup & Installation

### Prerequisites

| Tool | Download |
|---|---|
| VS Code | [code.visualstudio.com](https://code.visualstudio.com) |
| Python 3.11+ | [python.org/downloads](https://python.org/downloads) — check **Add to PATH** |
| PlatformIO IDE | VS Code Extensions → search `PlatformIO IDE` |
| Wokwi Simulator | VS Code Extensions → search `Wokwi Simulator` + valid license |

### 1. Create Python Virtual Environment

```bash
# Create venv inside project folder
python -m venv wokwi-env

# Activate — Windows
wokwi-env\Scripts\activate

# Activate — Mac / Linux
source wokwi-env/bin/activate

# Install PlatformIO inside venv
pip install platformio
```

### 2. Select Interpreter in VS Code

```
Ctrl+Shift+P
→ Python: Select Interpreter
→ Choose wokwi-env
```

### 3. Configure platformio.ini

```ini
[env:esp32dev]
platform  = espressif32
board     = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps  =
    WiFi
    WebServer
```

### 4. Configure wokwi.toml

```toml
[wokwi]
version  = 1
firmware = ".pio/build/esp32dev/firmware.bin"
elf      = ".pio/build/esp32dev/firmware.elf"

[[net.forward]]
from = "localhost:8180"
to   = "10.13.37.2:80"
```

### 5. Build the Project

```
Ctrl+Alt+B
```

Expected output:
```
=== [SUCCESS] Took ~18 seconds ===
```

---

## Running the Simulation

### Start Wokwi Simulator

```
Ctrl+Shift+P → Wokwi: Start Simulator
```

### Expected Serial Monitor Output

```
=============================
 Smart Hostel Energy Saver
=============================
Connecting to WiFi......
[OK] WiFi connected!
[OK] IP Address : http://10.13.37.2
[OK] Wokwi URL : http://localhost:8180
-----------------------------
Routes:
  GET  /          -> API info
  GET  /data      -> sensor JSON
  POST /relay     -> ?state=on|off
=============================
[OK] Web server started
```

---

## WiFi Dashboard

### Step 1 — Start the local file server

Open a terminal in the project root folder and run:

```bash
python -m http.server 3000
```

### Step 2 — Open the dashboard

```
http://localhost:3000/dashboard.html
```

> ⚠️ Always use `python -m http.server 3000` and open via
> `http://localhost:3000/dashboard.html` — never open `dashboard.html`
> directly by double-clicking (browser will block API requests).

### Dashboard Features

| Feature | Description |
|---|---|
| Relay status | Live ON / OFF with color indicator |
| PIR sensor | Heat detected / Idle |
| Motion count | Total detections since boot |
| LDR value | Raw ADC reading + Dark / Bright badge |
| Last event | Most recent state change with reason |
| Override banner | Appears when manual override is active |
| Force ON / OFF | Remote relay control buttons |

---

## API Reference

Base URL: `http://localhost:8180`

| Method | Endpoint | Description |
|---|---|---|
| GET | `/` | API status and available routes |
| GET | `/data` | Live sensor JSON |
| POST | `/relay?state=on` | Force relay ON (30 min override) |
| POST | `/relay?state=off` | Force relay OFF, cancel override |

### Sample `/data` Response

```json
{
  "relay": false,
  "motion": false,
  "ldr": 3797,
  "isDark": true,
  "motionCount": 4,
  "override": false,
  "lastEvent": "No motion for 5 min"
}
```

---

## Cost Estimate

### Prototype (breadboard)

```
Component total    ₹706
```

### Production unit (per room)

```
PCB Fabrication    ₹250
Project Enclosure  ₹80
5V Wall Adapter    ₹120
──────────────────────
Total per room     ₹1,150
```

### ROI — 100-room hostel

```
Deployment cost       ~₹1.15 lakh
Est. monthly savings   ₹8,000+
Break-even            < 15 months
```

---

## Future Improvements

| Phase | Feature |
|---|---|
| Phase 2 | 2-channel relay — fan and light controlled separately |
| Phase 2 | OLED status display mounted in the room |
| Phase 3 | Energy usage logging to cloud (ThingSpeak / Firebase) |
| Phase 3 | Warden mobile alerts via Telegram bot |
| Phase 3 | Central control panel for all hostel rooms |

---

*Simulated on Wokwi · Built with ESP32 + PlatformIO · Design Lab 2026*