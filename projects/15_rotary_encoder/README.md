# 15 — KY-040 Rotary Encoder (RP2040)

Standalone firmware for kit module #15 from the 37-in-1 sensor kit.

## Wiring (Pico)

| Module pin | Pico pin |
|---|---|
| VCC | 3V3 (see note) |
| GND | GND |
| signal | GP6=CLK GP7=DT GP8=SW |

> Modules marked 5V in the kit spec: digital outputs are 5V-tolerant-safe on
> RP2040 inputs only when the module output is push-pull TTL — open-drain
> comparator modules MUST use 3.3V Vcc or a divider. Analog modules ALWAYS
> run at 3.3V Vcc here.

## Behavior

Reports detent position; button resets to zero.

## Verification

Each click step changes count by exactly 1; no noise counts when idle.

## Implementation

Device class: `quadenc` — driver in `drivers/devices/`. The composition is in
`main.c`; class logic is host-verified bit-exact in `drivers/tests/`.
