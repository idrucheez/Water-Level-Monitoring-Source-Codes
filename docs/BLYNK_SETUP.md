# Blynk Setup

## Datastreams

Create the following virtual-pin datastreams:

| Virtual pin | Purpose | Suggested type |
|---|---|---|
| V0 | Water-level percentage | Double, 0–100 |
| V1 | Pump status | Integer, 0–1 |
| V2 | Pump override command | Integer, 0–1 |
| V3 | Ultrasonic distance | Double, 0–400 |
| V4 | Wi-Fi RSSI | Integer, approximately -100 to 0 |

## Events

Create events with these exact identifiers:

```text
critical_low
tank_full
```

The ESP32 sketch uses:

```cpp
Blynk.logEvent("critical_low", ...);
Blynk.logEvent("tank_full", ...);
```

## Suggested Dashboard

- Gauge connected to V0
- LED connected to V1
- Switch connected to V2
- Value Display connected to V3
- Value Display connected to V4
- SuperChart connected to V0 and optionally V3

## Implemented Override Behaviour

The V2 switch sends:

```text
OVERRIDE:1
```

or:

```text
OVERRIDE:0
```

The STM32 code interprets `OVERRIDE:1` as a request to force the relay ON. Setting V2 back to zero returns the system to automatic threshold control.

## Event Frequency Note

The ESP32 code checks alert conditions every two seconds. It does not contain a local latch or cooldown for Blynk events, so event logging may be requested repeatedly while a threshold remains active. Configure Blynk event limits appropriately.
