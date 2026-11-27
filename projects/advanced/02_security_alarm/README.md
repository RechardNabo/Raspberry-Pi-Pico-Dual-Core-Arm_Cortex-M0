# 02 — Security Alarm (integration: vibration + button + buzzer + LED)

Armed intrusion alarm finite state machine. The vibration sensor means
nothing unless armed — the integration contract:

- Short button press starts exit delay (ARMING, LED on)
- After `SA_EXIT_DELAY_TICKS` the system is ARMED
- A vibration edge while ARMED latches the ALARM (buzzer + LED)
- The latch survives the trigger stopping
- Only a long button hold (`SA_DISARM_HOLD_TICKS`) disarms

## Unit tests
3 tests: disarmed vibration is silent, arm-then-trigger sequence,
latch persistence + long-hold disarm.
