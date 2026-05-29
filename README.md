# ESP32 Snake Game

A snake game running on an ESP32 with an SH1106 OLED display, controlled from a browser over WiFi using WebSockets.

---

## Hardware

| Component | Details |
|---|---|
| MCU | ESP32 DevKit V1 (WROOM-32) |
| Display | SH1106 128x64 OLED (I2C) |
| Connection | USB for power + flashing |

### Wiring

| OLED Pin | ESP32 Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

---

## Software

**Framework:** Arduino via PlatformIO

**Libraries:**
- `olikraus/U8g2` — display driver
- `ottowinter/ESPAsyncWebServer-esphome` — async HTTP + WebSocket server
- `esphome/AsyncTCP-esphome` — required by ESPAsyncWebServer

---

## Project Structure

```
src/
├── main.cpp          — setup, loop, frame timing
├── display.h/.cpp    — all OLED drawing (u8g2 lives here only)
├── game.h/.cpp       — snake logic, state machine
└── wifi_server.h/.cpp — WiFi, HTTP server, WebSocket handler
```

---

## How It Works

```
ESP32 joins home WiFi (STA mode)
        ↓
IP address shown on OLED
        ↓
Open browser → type IP
        ↓
HTML page served with d-pad + keyboard controls
        ↓
Arrow key / button press → WebSocket message ("up/down/left/right")
        ↓
ESP32 receives message → updates snake direction
        ↓
Game loop runs at 200ms/frame → updates + redraws OLED
```

---

## Game States

| State | Description |
|---|---|
| `WAITING` | Snake visible, "press any key to start" shown |
| `PLAYING` | Snake moves, food spawns, score tracked |
| `DEAD` | Collision detected, game over overlay shown |

Transitions:
- `WAITING` → `PLAYING` : any arrow key
- `PLAYING` → `DEAD` : wall or self collision
- `DEAD` → `WAITING` : restart message from browser

---

## Controls

| Input | Action |
|---|---|
| Arrow keys | Change direction |
| D-pad buttons | Change direction (mobile/laptop) |
| Any key on game over | Restart |

---

## Configuration

In `wifi_server.cpp`:
```cpp
#define HOME_SSID "YourWiFiName"
#define HOME_PASS "YourWiFiPassword"
```

> ESP32 only supports 2.4GHz networks.

Frame speed in `main.cpp`:
```cpp
#define FRAME_MS 200   // lower = faster snake
```

---

## Building & Flashing

```bash
# build
pio run

# flash
pio run --target upload

# monitor
pio device monitor
```

Clean build if switching libraries:
```bash
pio run --target clean
pio run --target upload
```

---

## Concepts Covered

- I2C communication (OLED)
- WiFi STA mode
- HTTP server serving HTML to browser
- WebSocket for real-time bidirectional communication
- Non-blocking game loop with `millis()`
- State machine (WAITING / PLAYING / DEAD)
- Modular C++ with separation of concerns