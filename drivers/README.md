# RP2040 Peripheral Driver Library (Raspberry Pi Pico)

Register-level drivers, host-verified bit-exact. Same modular architecture
as the PIC32MX795 / dsPIC30F6014 / ESP32 libraries in the sibling repos.

## Modules (28 host tests green — gcc mirror)

| Peripheral | Channels | Tests | What the tests pin down |
|---|---|---|---|
| GPIO (SIO) | pins 0-29 | 5 | atomic SET/CLR/XOR alias semantics — the dual-core-safe idiom |
| UART | 2 | 5 | PL011 integer+1/64 fractional divisor, LCR_H-before-enable order, FIFO flag gating |
| SPI master | 2 | 5 | cascaded CPSDVSR×(1+SCR) divisor never overclocks, orthogonal SPO/SPH mode bits, timeout fault path |
| I2C master | 2 | 4 | DW internal-latency absorption (7/9 clocks), counts-while-disabled rule, 7-bit address limit |
| PWM | 8 slices | 5 | 16-bit TOP overflow forces integer clock divider — caught by the tests during development |
| ADC | 5 ch | 4 | AINSEL encoding, START_ONCE/READY conversion model, integer millivolt math |

## The RP2040-specific traps this library absorbs

1. **PWM TOP is only 16-bit** — at 125 MHz, frequencies below ~2 kHz are
   impossible without the clock divider. The driver picks the smallest
   divider bringing the period into range; asking for 1 kHz with div=1
   silently overflowed until the tests caught it.
2. **UART divisor is two registers** (IBRD + 6-bit FBRD in 1/64 steps) —
   most baud rates come out exact, but LCR_H must be written while the
   UART is disabled or the silicon ignores bits.
3. **SPI has two cascaded divisors** — CPSDVSR (even only, 2-254) × (1+SCR).
   The wrong pairing overclocks the slave or wastes half the bandwidth.
4. **DW I2C has mandatory internal latency** — SCL counts must absorb
   ~7/~9 IC clocks or the actual frequency comes out wrong, and counts
   are only writable while the block is disabled.
5. **SIO atomic aliases** — SET/CLR/XOR are the correct multicore idiom;
   RMW on GPIO_OUT races across cores.

## Honest scope

- Host-verified only; Pico SDK target compilation not yet proven
- PWM register model simplifies the A/B-per-slice interleave into arrays
- Out of scope this pass: PIO (the signature peripheral — deserves its
  own dedicated pass), USB device, DMA, interpolators, timers/alarms
