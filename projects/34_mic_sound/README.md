# 34 — Mic Sound Threshold (RP2040)

Standalone firmware for kit module #34 from the 37-in-1 sensor kit.

## Wiring (Pico)

| Module pin | Pico pin |
|---|---|
| VCC | 3V3 (see note) |
| GND | GND |
| signal | GP12=OUT |

> Modules marked 5V in the kit spec: digital outputs are 5V-tolerant-safe on
> RP2040 inputs only when the module output is push-pull TTL — open-drain
> comparator modules MUST use 3.3V Vcc or a divider. Analog modules ALWAYS
> run at 3.3V Vcc here.

## Behavior

Digital clap detection with the comparator output; count over UART.

## Verification

Clap: event per clap, pot-adjusted sensitivity.

## Implementation

Device class: `digin` — driver in `drivers/devices/`. The composition is in
`main.c`; class logic is host-verified bit-exact in `drivers/tests/`.
