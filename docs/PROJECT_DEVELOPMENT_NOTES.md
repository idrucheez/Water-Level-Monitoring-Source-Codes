# Smart Water Level Monitoring & Automatic Pump Control System

## Project Overview

This is an embedded systems project built for the course **EFB 2073/EEB 2083 – Microprocessors & Computer Architecture** at Universiti Teknologi PETRONAS (UTP), January 2026 Semester. The project is worth 32% of the overall course marks and required designing, developing, and implementing a functional embedded system prototype using an STM32 Nucleo board.

The system monitors water level in a tank in real-time using an ultrasonic sensor, automatically controls a water pump via a relay module, displays live status on an OLED screen, provides audible alerts, and enables remote IoT monitoring through a smartphone app using a secondary ESP32 microcontroller connected to the Blynk IoT cloud platform.

## Problem Statement

Manual water tank monitoring is inefficient and unreliable. Homeowners, farms, and small facilities often face tank overflow or dry-running pumps because there is no automated way to track water levels and control the pump accordingly. This project solves that with a low-cost automated monitoring and control system.

## Project Objectives

1. Monitor water level in real-time using ultrasonic distance sensing with noise filtering
2. Automatically control a water pump based on configurable water level thresholds
3. Display system status (water level, distance, pump state) on an OLED screen
4. Provide audible buzzer alerts when the tank is full or critically low
5. Enable remote monitoring via smartphone using the Blynk IoT platform over WiFi
6. Demonstrate embedded hardware-software integration using C/C++ on the Mbed framework

## System Architecture

The system uses **two microcontrollers working together**:

1. **STM32 Nucleo-F401RE** (main controller) — handles all real-time tasks: reading the ultrasonic sensor, computing water level, controlling the relay/pump, driving the buzzer, and updating the OLED display. Programmed in C/C++ using the ARM Mbed framework (compiled via Keil Studio Cloud).

2. **ESP32 DevKit (CP2102)** (WiFi/IoT bridge) — receives sensor data from the STM32 over UART and forwards it to the Blynk IoT cloud platform via WiFi/HTTP. Programmed using Arduino IDE with the Blynk library.

The two boards communicate over a UART serial link at 9600 baud. Data is sent as a simple formatted string: `L:45.2,D:12.3,P:1` where L = water level %, D = distance in cm, P = pump status (1=ON, 0=OFF).

## Hardware Components

| Component | Model/Spec | Purpose |
|---|---|---|
| Microcontroller (main) | STM32 Nucleo-F401RE (ARM Cortex-M4, 84MHz, 512KB flash, 96KB RAM) | Main control unit |
| Microcontroller (WiFi bridge) | ESP32 DevKit (CP2102 USB-serial chip) | IoT/WiFi connectivity |
| Ultrasonic sensor | HC-SR04 | Measures distance to water surface (2cm–400cm range) |
| OLED display | SSD1306, 128x32 pixels, I2C | Shows water level, distance, pump status |
| Relay module | SRD-12VDC-SL-C (12V, with onboard 2N2222 transistor driver, optocoupler, flyback diode) | Switches the 12V pump circuit |
| Water pump | 12V DC submersible pump | Pumps water into the tank |
| Buzzer | Piezo buzzer | Audible alerts |
| Power supply | 12V (3x 18650 Li-ion cells in series, ~11.1V nominal) | Powers relay coil and pump |

## Pin Connections (STM32 Nucleo-F401RE)

| Pin | Function | Connected To | Protocol |
|---|---|---|---|
| PA_0 | Trigger output | HC-SR04 TRIG | GPIO |
| PA_1 | Echo input | HC-SR04 ECHO (via voltage divider, 5V→3.3V) | GPIO |
| PB_9 | I2C SDA | OLED SDA | I2C |
| PB_8 | I2C SCL | OLED SCL | I2C |
| D4 | OLED reset | OLED RST | GPIO |
| PA_6 | Buzzer output | Buzzer positive | GPIO |
| PB_6 | Relay signal | Relay module IN | GPIO |
| PA_9 | UART TX | ESP32 GPIO16 (RX) | UART |
| PA_10 | UART RX | ESP32 GPIO17 (TX) | UART |

**Important note:** PA_7 was originally used for the relay signal but caused issues (pin conflict, output stuck below 1V despite being set HIGH in software). Switched to PB_6, which resolved the issue.

### ESP32 Pin Connections

| Pin | Function | Connected To |
|---|---|---|
| GPIO 16 | UART RX | STM32 PA_9 (TX) |
| GPIO 17 | UART TX | STM32 PA_10 (RX) |
| GND | Ground | STM32 GND (shared ground required) |

## Circuit Notes

- The HC-SR04 ECHO pin outputs 5V, but STM32 GPIO is 3.3V tolerant only — a voltage divider (1kΩ/2kΩ) steps down the ECHO signal.
- The relay module (SRD-12VDC-SL-C) has an onboard optocoupler and 2N2222 transistor driver, so no external driver circuit is needed. It has 3 input pins (DC+, DC-, IN) and 3 output screw terminals (COM, NO, NC). The jumper on the module must be set to **high-level trigger** mode.
- A first relay module (5V type) was accidentally destroyed by connecting it directly to a 12V battery, which fried the optocoupler/transistor driver (power LED stayed on but signal LED stopped responding). The project switched to a proper 12V relay module afterward.
- Common ground between the STM32, the 12V power supply, and the ESP32 is essential — this was a major early debugging issue.

## Firmware Details (STM32 - Mbed C/C++)

### Ultrasonic Sensing
- Sends a 10µs trigger pulse, measures echo duration via hardware `Timer`, converts to distance by dividing by 58 (µs to cm conversion for HC-SR04)
- Takes 5 consecutive readings 30ms apart, discards invalid readings (outside 0-400cm), averages the rest for noise reduction
- Distance is linearly mapped to water level %: tank empty distance (18.5cm, measured for the actual test container) = 0%, tank full distance (3.0cm) = 100%

### Pump Control Logic
- Threshold-based automatic control: pump ON when level < 90%, pump OFF when level ≥ 90%
- Safety shutoff: pump forced OFF if sensor readings are invalid (sensor error state)
- Manual override capability was implemented (via Blynk app toggle) but later removed as unnecessary for the final version — pragmatic decision since automatic control covered all rubric requirements

### Buzzer Alerts
- Single 2-second beep (not continuous) when tank reaches full (≥95%) or critically low (≤10%)
- Boolean flags (`full_buzzed`, `low_buzzed`) ensure each alert fires only once per threshold crossing, resetting when level moves back within normal range

### OLED Display
- Custom-built 5x7 pixel font renderer (ASCII 32-127) since no external display library was used — characters are drawn directly into a 512-byte frame buffer (128x32 pixels, 4 pages of 8-bit columns)
- Displays 3 lines: water level % (with a graphical bar), distance/status message, and pump state
- SSD1306 initialization sequence sent via I2C at 100kHz, address 0x3C

### UART to ESP32
- STM32 sends `L:<level>,D:<distance>,P:<pump_status>\n` every ~500ms
- Note: an important debugging lesson was that in this project's Mbed OS version (1.7.16, "classic" Mbed 2), a globally-declared `Serial` object caused an "identifier undefined" compiler error. The fix was declaring the `Serial` object **locally inside `main()`** rather than globally.

## Firmware Details (ESP32 - Arduino/Blynk)

- Uses `HardwareSerial` (UART2, GPIO16/17) to receive data from STM32
- Parses the `L:...,D:...,P:...` string format
- Sends parsed values to Blynk virtual pins:
  - V0 = Water Level (gauge widget)
  - V1 = Pump Status (LED widget)
  - V3 = Distance (value display)
  - V4 = WiFi signal strength (optional)
- Sends Blynk event log notifications (`critical_low`, `tank_full`) for push notifications to the phone
- Runs `Blynk.run()` and a `BlynkTimer` to send data every 2 seconds

## IoT Integration (Blynk)

- Platform: Blynk IoT (blynk.cloud), free tier
- Template created with datastreams for water level (double, 0-100), pump status (integer, 0/1), distance (double, 0-400)
- Events configured for critical low and tank full notifications
- Mobile app dashboard includes: Gauge (water level), LED (pump status), Value Display (distance), SuperChart (historical water level logging)
- Communication: ESP32 connects to home WiFi (2.4GHz only — ESP32 cannot use 5GHz), then talks to Blynk cloud over WiFi/HTTP

## Development Tools Used

- **Keil Studio Cloud** (studio.keil.arm.com) — used to compile and flash the STM32 Mbed C/C++ firmware. Target board: NUCLEO-F401RE. Mbed OS version: 1.7.16 (classic Mbed 2 / "mbed.h" style, not Mbed OS 5/6).
- **Arduino IDE** — used to compile and flash the ESP32 firmware, with the ESP32 board package and Blynk library installed.

## Key Debugging Lessons (useful for README/troubleshooting docs)

1. **Draw-before-sprintf bug**: an early version of the OLED code called `draw_text()` before `sprintf()` filled the display strings, rendering garbage. Fixed by ensuring all `sprintf` calls happen before any `draw_text` calls.
2. **Relay pin PA_7 failure**: PA_7 was set HIGH in software (confirmed via test code and onboard LED blinking correctly) but measured less than 1V on a multimeter — likely a pin conflict (PA_7 is also SPI1_MOSI). Switched to PB_6, which resolved the issue immediately.
3. **Dead relay module**: a 5V relay module was destroyed by connecting it directly to a 12V battery for testing — no red power LED, and the board was warm to the touch, indicating internal short/failure. Replaced with a proper 12V relay module (SRD-12VDC-SL-C).
4. **ESP-01S abandoned**: initially attempted to use an ESP-01S WiFi module communicating via raw AT commands over UART, but the module was found to be dead (no power LED, warm to touch) after accidental direct 12V connection. Switched to an ESP32 DevKit running the Blynk library directly instead of AT commands — a simpler and more reliable architecture.
5. **Global Serial declaration issue in Mbed OS 2**: declaring `Serial espSerial(...)` as a global variable caused a "identifier undefined" compile error in Keil Studio Cloud, specific to this Mbed version. Fixed by declaring the Serial object locally inside `main()`.
6. **Tank calibration**: initial code used generic tank dimensions (50cm empty, 5cm full); the actual test container was measured at 18.5cm (empty) and ~3cm (full), and the code was recalibrated accordingly.

## Testing Results

The system was tested using an 18.5cm tall glass/plastic container filled with water at various levels, verified against OLED and Blynk app readings.

| Water Level | Sensor Distance | Displayed Level | Pump State | Buzzer |
|---|---|---|---|---|
| Empty | 18.5 cm | 0% | ON (auto) | Silent |
| Half | ~10.8 cm | 50% | ON (auto) | Silent |
| Full | ~3.0 cm | 100% | OFF (auto) | 2s beep |
| Critical low | ~17.0 cm | 10% | ON (auto) | 2s beep |

The ultrasonic sensor provided consistent readings with approximately ±1cm accuracy after implementing the 5-sample averaging filter. The Blynk mobile app dashboard updated within 2-3 seconds of each measurement cycle.

## Project Deliverables (per course requirements)

1. Functional hardware prototype (breadboard-based)
2. Demonstration video (hardware setup, system operation, feature walkthrough)
3. Web-based project report (Google Sites) covering: Background, Hardware Design, Firmware/Software Development, Project Outcomes and Accomplishments
4. Source code (this repository)

## Suggested Repository Structure

```
water-level-monitor/
├── README.md
├── stm32_firmware/
│   └── main.cpp                 (STM32 Nucleo-F401RE, ARM Mbed C/C++)
├── esp32_firmware/
│   └── esp32_blynk_bridge.ino   (ESP32, Arduino IDE, Blynk library)
├── docs/
│   ├── system_block_diagram.png
│   ├── program_flowchart.png
│   ├── wiring_diagram.png
│   └── hardware_photos/
└── LICENSE
```

## Future Improvements (as identified during the project)

- Add a water flow sensor to measure actual volume pumped
- Implement SMS alerts as a backup when WiFi is unavailable
- Add a secondary/redundant sensor for improved accuracy
- Support multiple tanks
- Add battery backup with solar charging for outdoor deployment

## Course & Institutional Context

- **Course:** EFB 2073/EEB 2083 – Microprocessors & Computer Architecture
- **Institution:** Universiti Teknologi PETRONAS (UTP)
- **Semester:** January 2026
- **Instructor:** Assoc. Prof. Dr. Mohd Zuki Yusoff
- **Hardware platform constraint:** Must use NUCLEO-F401RE or NUCLEO-F411RE, programmed in ARM Mbed (Keil µVision was prohibited for this specific project, unlike earlier practical assignments in the same course)
- **Minimum technical requirements met:** Sensor input (ultrasonic), actuator control (pump/relay), real-time timing (timer-based echo measurement), display interface (OLED), communication interface (I2C, UART, WiFi/HTTP), control logic (threshold-based automation) — exceeds the minimum 4 required elements

## Author

Idriss Rama Salim — Electrical & Electronics Engineering student, Universiti Teknologi PETRONAS
GitHub: github.com/Idrucheez
LinkedIn: linkedin.com/in/idriss-rama-192046253