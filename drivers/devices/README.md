# Kit Device Drivers — 37-in-1 Sensor Kit on RP2040

Device drivers built on the peripheral layer. The kit's 37 modules collapse
into interface classes — one driver per class, one configuration per module.
This is the anti-copy-paste architecture: a new kit module is a config
struct, not a new driver.

## Class coverage map

| Class | Driver | Kit modules covered |
|---|---|---|
| Digital input | `dig_in` | #28 button, #35/#25 reed, #26 tilt, #23 mercury, #30 vibration, #24 hall switch, #5 IR proximity, #13 line, #7 obstacle, #10 touch, #20 broken-light, #11 flame D0, #34 sound D0, #31 knock D0 |
| Analog input | `analog_in` | #1 joystick X/Y, #29 LDR, #21/#32 LM35 temp, #14/#33 linear hall, #9 sound envelope, #8 heartbeat PPG, #11 flame A0, #31 knock A0 |
| Digital output | `dig_out` | #6 relay (active-LOW!), #16 active buzzer, #3 laser, #22 dual LED, #27 flash LED |
| PWM output | `pwm_out` *(next batch)* | #12 RGB LED, #18 passive buzzer |
| Quadrature | `quad_enc` *(next batch)* | #15 rotary encoder |
| 1-wire bitbang | *(needs timer driver first)* | #4 DHT11, #19 DS18B20 |
| IR capture | *(needs input capture)* | #2 IR receiver |

## Device contract

- Drivers never own pins — configuration structs carry pin/channel bindings
- Active-level is explicit config (the relay is active-LOW; assuming
  active-HIGH is the classic way to weld contacts at power-on)
- `init()` validates bindings against BSP facts; read/write return status
- Scaling to engineering units happens at the device layer, in integer math
