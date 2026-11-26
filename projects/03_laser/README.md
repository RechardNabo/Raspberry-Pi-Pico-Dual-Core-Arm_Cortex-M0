# 03 — Laser Diode (RP2040)

Standalone firmware for kit module #3 from the 37-in-1 sensor kit.

## Wiring (Pico)

| Module pin | Pico pin |
|---|---|
| VCC | 3V3 (see note) |
| GND | GND |
| signal | GP18=S |

> Modules marked 5V in the kit spec: digital outputs are 5V-tolerant-safe on
> RP2040 inputs only when the module output is push-pull TTL — open-drain
> comparator modules MUST use 3.3V Vcc or a divider. Analog modules ALWAYS
> run at 3.3V Vcc here.

## Behavior

Blinks the 650nm laser at 1Hz, 50% duty.

## Verification

Laser dot visibly pulses; measure with LDR module for closed-loop demo.

## Implementation

Device class: `digout` — driver in `drivers/devices/`. The composition is in
`main.c`; class logic is host-verified bit-exact in `drivers/tests/`.
