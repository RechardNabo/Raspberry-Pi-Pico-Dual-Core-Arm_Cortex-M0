# 01 — Climate Guard (integration: LM35 + relay + buzzer)

Thermostat control plus independent overheat alarm. The LM35 analog
temperature drives two paths that must never interfere:

- **Control path** — heating relay engages below `CG_HEAT_ON_COUNT`,
  releases above `CG_HEAT_OFF_COUNT` (hysteresis — no flapping)
- **Alarm path** — buzzer sounds above `CG_ALARM_COUNT` regardless of
  relay state (runaway / sensor-fault indication)

## Wiring

| Module | Pin |
|---|---|
| LM35 analog out | ADC channel 0 input (kit table) |
| Relay control | kit default relay pin |
| Active buzzer | kit default buzzer pin |

## Unit tests

3 tests pin: power-on deadband safety, engage/hold/release hysteresis
cycle, alarm-path independence from relay state.
