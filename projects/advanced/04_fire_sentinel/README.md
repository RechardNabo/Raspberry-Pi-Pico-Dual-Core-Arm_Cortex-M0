# 04 — Fire Sentinel (integration: flame + LM35 + buzzer + LED)

Correlated fire detection. Single-sensor fire alarms are notorious for
false positives — sunlight on the flame sensor, a hot room without fire.
The fusion rule:

| flame | hot | result |
|-------|-----|--------|
| yes   | yes | **ALARM** — buzzer + LED |
| yes   | no  | WARNING — LED only |
| no    | yes | WARNING — LED only |
| no    | no  | CLEAR |

## Unit tests
3 tests: flame-alone rejection, correlated alarm, heat-only warning +
return to clear.
