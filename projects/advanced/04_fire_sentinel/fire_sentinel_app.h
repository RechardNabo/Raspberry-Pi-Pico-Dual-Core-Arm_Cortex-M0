/* fire_sentinel_app.h — correlated fire detection with false-alarm rejection.
 * Integration: flame sensor + LM35 temperature are FUSED:
 *   flame AND hot  → full ALARM (buzzer + LED)
 *   exactly one    → WARNING (LED only) — worth attention, not evacuation
 *   neither        → CLEAR
 * The fusion contract: a flame flicker at room temperature (sunlight,
 * lighter) must never sound the buzzer; sustained heat without visible
 * flame is a warning, not an alarm. */
#ifndef FIRE_SENTINEL_APP_H
#define FIRE_SENTINEL_APP_H

#include <stdint.h>
#include <stdbool.h>
#include "../../../drivers/devices/analog_in/analog_in.h"
#include "../../../drivers/devices/dig_in/dig_in.h"
#include "../../../drivers/devices/dig_out/dig_out.h"

#define FS_HOT_COUNT  2867U   /* raw count above which temperature confirms fire */

typedef enum {
    FS_CLEAR = 0,
    FS_WARNING,
    FS_ALARM
} fs_level_t;

typedef struct {
    dig_in_t    flame;
    analog_in_t temp;
    dig_out_t   buzzer;
    dig_out_t   led;
    fs_level_t  level;
} fire_sentinel_app_t;

void fire_sentinel_app_init(fire_sentinel_app_t *app, uint8_t flame_pin,
                            uint8_t adc_channel, uint8_t buzzer_pin, uint8_t led_pin);
void fire_sentinel_app_step(fire_sentinel_app_t *app);
fs_level_t fire_sentinel_level(const fire_sentinel_app_t *app);

#endif
