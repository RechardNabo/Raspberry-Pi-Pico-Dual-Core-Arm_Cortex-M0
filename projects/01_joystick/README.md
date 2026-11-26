# 01 — PS2 Joystick (RP2040)

Standalone firmware for kit module #1 from the 37-in-1 sensor kit.

## Wiring (Pico)

| Module pin | Pico pin |
|---|---|
| VCC | 3V3 (see note) |
| GND | GND |
| signal | GP26=VRx GP27=VRy GP8=SW |

> Modules marked 5V in the kit spec: digital outputs are 5V-tolerant-safe on
> RP2040 inputs only when the module output is push-pull TTL — open-drain
> comparator modules MUST use 3.3V Vcc or a divider. Analog modules ALWAYS
> run at 3.3V Vcc here.

## Behavior

Reads both axes as permille of travel and the button state; reports over UART console.

## Verification

Move stick: X/Y values track 0-1000‰, center ~500.

## Implementation

Device class: `analog2` — driver in `drivers/devices/`. The composition is in
`main.c`; class logic is host-verified bit-exact in `drivers/tests/`.
