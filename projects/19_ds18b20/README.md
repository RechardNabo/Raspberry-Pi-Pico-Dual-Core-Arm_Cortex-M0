# 19 — DS18B20 Precision Temp (RP2040)

Standalone firmware for kit module #19 from the 37-in-1 sensor kit.

## Wiring (Pico)

| Module pin | Pico pin |
|---|---|
| VCC | 3V3 (see note) |
| GND | GND |
| signal | GP22=SIG |

> Modules marked 5V in the kit spec: digital outputs are 5V-tolerant-safe on
> RP2040 inputs only when the module output is push-pull TTL — open-drain
> comparator modules MUST use 3.3V Vcc or a divider. Analog modules ALWAYS
> run at 3.3V Vcc here.

## Behavior

Reads 12-bit temperature every second, reports centi-degrees.

## Verification

Pinch the sensor: reading climbs toward body temp ~31-34C.

## Implementation

Device class: `ds18b20` — driver in `drivers/devices/`. The composition is in
`main.c`; class logic is host-verified bit-exact in `drivers/tests/`.
