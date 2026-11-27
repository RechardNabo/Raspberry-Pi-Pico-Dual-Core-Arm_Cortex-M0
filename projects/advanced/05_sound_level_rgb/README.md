# 05 — Sound Level RGB (integration: mic + RGB LED)

Sound level color meter with peak-hold decay:

- quiet → blue, moderate → green, loud → red
- the displayed level is the **decayed peak**, never the instantaneous
  sample — the meter stays readable after a spike
- decay is linear at `SL_DECAY_PER_TICK` counts per tick

## Unit tests
3 tests: silence classification, loud classification, exact decay
arithmetic (spike − decay per tick, not instant drop).
