# 17 — Magic Cup LED+LDR (RP2040)

Standalone firmware for kit module #17 from the 37-in-1 sensor kit.

## Wiring (Pico)

| Module pin | Pico pin |
|---|---|
| VCC | 3V3 (see note) |
| GND | GND |
| signal | GP26=A0 |

> Modules marked 5V in the kit spec: digital outputs are 5V-tolerant-safe on
> RP2040 inputs only when the module output is push-pull TTL — open-drain
> comparator modules MUST use 3.3V Vcc or a divider. Analog modules ALWAYS
> run at 3.3V Vcc here.

## Behavior

Reports LDR permille as the internal LED ball tilts.

## Verification

Tilting changes reading smoothly.

## Implementation

Device class: `analog` — driver in `drivers/devices/`. The composition is in
`main.c`; class logic is host-verified bit-exact in `drivers/tests/`.
