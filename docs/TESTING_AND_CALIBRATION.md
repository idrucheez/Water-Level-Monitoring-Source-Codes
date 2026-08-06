# Testing and Calibration

## Tank Calibration Values Used by the Firmware

```text
Empty distance: 18.5 cm
Full distance: 3.0 cm
```

The level is calculated linearly between those limits:

```text
Level % =
(Empty distance - measured distance)
------------------------------------------------ × 100
(Empty distance - full distance)
```

Values at or below 3.0 cm are clamped to 100%. Values at or above 18.5 cm are clamped to 0%.

## Measurement Filtering

For each update, the STM32:

1. Takes five measurements.
2. Separates samples by 30 ms.
3. Rejects values at or below zero or at/above 400 cm.
4. Averages the remaining valid measurements.
5. Returns an error if no valid sample remains.

## Bench Test Sequence

1. Power the STM32 without the pump connected.
2. Verify OLED initialization.
3. Move a flat target between approximately 3 cm and 18.5 cm.
4. Confirm the displayed percentage changes correctly.
5. Confirm UART records appear at the ESP32.
6. Confirm V0, V1, V3, and V4 update in Blynk.
7. Connect the relay without the pump and verify relay switching.
8. Connect the pump using a fused, correctly rated power circuit.
9. Test the empty, half, full, and critical-low conditions.
10. Confirm the sensor-error state forces the relay off.

## Expected Threshold Behaviour

| Level | Automatic pump state |
|---|---|
| Below 90% | ON |
| 90% or above | OFF |
| Invalid sensor result | OFF |

## Buzzer Behaviour

- A 2-second alert occurs when level reaches at least 95%.
- A 2-second alert occurs when level reaches 10% or lower.
- A flag prevents repeated buzzer activation until the level exits and re-enters the threshold.
- A repeating pattern is used in the sensor-error state.

## Documented Results

| Condition | Distance | Level | Pump | Buzzer |
|---|---:|---:|---|---|
| Empty | 18.5 cm | 0% | ON | Low-level alert when threshold is entered |
| Half | Approximately 10.8 cm | Approximately 50% | ON | Silent |
| Full | Approximately 3.0 cm | 100% | OFF | 2-second alert |
| Critical low | Approximately 17.0 cm | Approximately 10% | ON | 2-second alert |

The development notes report approximately ±1 cm consistency after sample averaging.
