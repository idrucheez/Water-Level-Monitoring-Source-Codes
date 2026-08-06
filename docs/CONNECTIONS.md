# Hardware Connections

## STM32 Nucleo-F401RE

| STM32 pin | Connected device | Function |
|---|---|---|
| PA_0 | HC-SR04 TRIG | 10 µs trigger output |
| PA_1 | HC-SR04 ECHO through voltage divider | Echo pulse input |
| PB_9 | SSD1306 SDA | I2C data |
| PB_8 | SSD1306 SCL | I2C clock |
| D4 | SSD1306 RST | Display reset |
| PA_6 | Buzzer positive/control | Audible alert |
| PB_6 | Relay IN | Pump control |
| PA_9 | ESP32 GPIO16 | STM32 UART TX to ESP32 RX |
| PA_10 | ESP32 GPIO17 | STM32 UART RX from ESP32 TX |
| GND | ESP32 GND and power-system signal ground | Shared reference |

## ESP32

| ESP32 pin | Connected device |
|---|---|
| GPIO16 | STM32 PA_9 |
| GPIO17 | STM32 PA_10 |
| GND | STM32 GND |

## HC-SR04 Voltage Divider

The HC-SR04 ECHO output is approximately 5 V, while the STM32 input is a 3.3 V logic input.

A 1 kΩ / 2 kΩ divider may be wired as:

```text
HC-SR04 ECHO ---- 1 kΩ ----+---- STM32 PA_1
                            |
                           2 kΩ
                            |
                           GND
```

The divided voltage is approximately:

```text
5 V × 2 kΩ / (1 kΩ + 2 kΩ) ≈ 3.33 V
```

## OLED

```text
OLED VCC -> supply appropriate for the module
OLED GND -> common ground
OLED SDA -> PB_9
OLED SCL -> PB_8
OLED RST -> D4
```

The firmware uses:

- I2C frequency: 100 kHz
- OLED address: 0x3C
- Resolution: 128×32

## Relay and Pump

The firmware drives PB_6 HIGH when the pump should run.

```text
PB_6 -> relay module IN
Relay COM/NO -> pump power circuit
```

The exact relay supply and contact wiring must match the actual relay module. The documented project used a 12 V relay module with onboard drive circuitry.

Do not route pump current through the STM32.

## Power Safety

- Do not connect the 3S lithium battery directly to STM32 or ESP32 logic rails.
- Use USB or properly regulated supplies for the microcontrollers.
- Use a protected 3S battery pack and a correct 12.6 V lithium-ion charger.
- Add a fuse near the battery positive terminal.
- Confirm the relay coil voltage before applying power.
- Keep the pump/relay current path separate from low-current logic wiring.
- Join grounds where a shared signal reference is required.
- Protect electronics from water and condensation.
