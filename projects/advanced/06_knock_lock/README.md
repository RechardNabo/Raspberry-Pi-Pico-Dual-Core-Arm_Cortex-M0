# 06 — Knock Lock (integration: knock + relay + LED)

Secret-knock access control:

- exactly `KL_REQUIRED` (3) knocks within `KL_WINDOW_TICKS` unlocks
- sparse knocks never accumulate — the sliding window expires partial
  patterns back to zero
- the relay relocks automatically after `KL_UNLOCK_TICKS`

## Unit tests
3 tests: correct pattern unlocks, partial-pattern window expiry,
automatic relock.
