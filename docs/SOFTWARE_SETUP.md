# Software Setup

## STM32 Firmware

File:

```text
stm32_firmware/main.cpp
```

The source uses classic Mbed APIs such as:

- `Serial`
- `wait()`
- `wait_us()`
- `Timer::read_us()`
- `DigitalOut`
- `DigitalIn`
- `I2C`

It was documented as being developed with Mbed OS 1.7.16 / classic Mbed 2 style in Keil Studio Cloud.

### Procedure

1. Create or open a compatible Mbed project for `NUCLEO-F401RE`.
2. Ensure `mbed.h` is available.
3. Replace the project's application source with the supplied `main.cpp`, or add it as the application source.
4. Select the NUCLEO-F401RE target.
5. Build the project.
6. Flash the compiled firmware to the board.
7. Verify the OLED startup message and UART output.

The supplied source has intentionally not been modified.

## ESP32 Firmware

File:

```text
esp32_firmware/esp32_blynk.ino
```

### Requirements

- Arduino IDE
- ESP32 board package
- Blynk library
- A 2.4 GHz Wi-Fi network
- A configured Blynk template/device

### Procedure

1. Open the sketch in Arduino IDE.
2. Replace and revoke the credentials embedded in the original sketch before publishing or deploying it.
3. Select the appropriate ESP32 Dev Module board.
4. Select the correct COM port.
5. Compile and upload.
6. Open Serial Monitor at 115200 baud.
7. Confirm connection to Wi-Fi and Blynk.
8. Confirm UART messages are arriving from the STM32.

## Serial Settings

### STM32-to-ESP32 UART

```text
Baud rate: 9600
Format: 8 data bits, no parity, 1 stop bit
STM32 TX: PA_9
STM32 RX: PA_10
ESP32 RX: GPIO16
ESP32 TX: GPIO17
```

### ESP32 USB Debugging

```text
Baud rate: 115200
```
