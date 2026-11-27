# 03 — Auto Light (integration: LDR + button + relay)

Dusk-to-dawn automatic lighting with temporary manual override:

- Dark (below `AL_DARK_COUNT`) turns the relay ON, bright
  (`AL_BRIGHT_COUNT`) turns it OFF — hysteresis between
- A button press inverts the output as a manual override
- The override is **automatically cleared at the next day/night
  transition** — a forgotten manual input can never defeat the
  following automatic cycle

## Unit tests
3 tests: hysteresis cycle, override inversion and toggling,
auto-clear on transition.
