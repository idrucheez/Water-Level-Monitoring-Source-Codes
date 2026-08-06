# Troubleshooting and Development Lessons

## OLED displays incorrect or garbage text

A previous implementation drew text before the display strings were populated. The final supplied source uses `sprintf()` before `draw_text()`.

## Relay output problem on PA_7

PA_7 was originally tested as the relay-control output but measured below 1 V despite being set HIGH. The relay signal was moved to PB_6, which resolved the problem.

## Relay does not switch

Check:

- Relay coil/supply voltage
- Trigger-mode jumper
- PB_6 output
- Common ground
- Whether the module expects active HIGH or active LOW
- Whether the relay module was previously exposed to overvoltage

The documented project damaged an earlier 5 V relay module by applying 12 V directly.

## No UART data on ESP32

Check:

```text
STM32 PA_9 -> ESP32 GPIO16
STM32 PA_10 -> ESP32 GPIO17
GND -> GND
Baud rate -> 9600
```

Also note that the preserved `main.cpp` contains both:

```cpp
DigitalOut esp_tx(PA_9);
```

and a `Serial` object using PA_9/PA_10 inside `main()`. The source has not been changed, but this dual allocation is worth checking if compilation or pin-operation issues recur.

## ESP32 receives data but Blynk does not update

Check:

- 2.4 GHz Wi-Fi availability
- Blynk template and device configuration
- Virtual-pin assignments
- Blynk event identifiers
- Auth token validity
- Serial Monitor output at 115200 baud

## Pump remains ON in manual mode

This matches the implemented override logic. `OVERRIDE:1` forces the relay ON. `OVERRIDE:0` returns to automatic mode.

## Repeated phone notifications

The ESP32 sketch requests Blynk event logging every two seconds while an alert condition remains true. The code does not implement an event latch or cooldown.

## Sensor reports errors

Check:

- HC-SR04 supply
- Trigger and echo wiring
- Echo voltage divider
- Sensor orientation
- Water-surface turbulence
- Minimum sensor distance
- Condensation on the sensor
