/* auto_light_app.h — dusk-to-dawn lighting with manual override.
 * Integration: LDR analog level + button control the lighting relay:
 *   dark (below LOW)  → relay ON   (night: lights needed)
 *   bright (above HIGH) → relay OFF (day)
 *   button edge toggles an override that INVERTS the output,
 *   automatically cleared at the next day/night transition.
 * The fusion contract: override is temporary by design — a forgotten
 * manual input can never defeat the next automatic cycle. */
#ifndef AUTO_LIGHT_APP_H
#define AUTO_LIGHT_APP_H

#include <stdint.h>
#include <stdbool.h>
#include "../../../drivers/devices/analog_in/analog_in.h"
#include "../../../drivers/devices/dig_in/dig_in.h"
#include "../../../drivers/devices/dig_out/dig_out.h"
#include "../../lib/hysteresis.h"
#include "../../lib/edge_count.h"

#define AL_DARK_COUNT    1200U   /* below: night */
#define AL_BRIGHT_COUNT  2000U   /* above: day */

typedef struct {
    analog_in_t  sensor;
    dig_in_t     button;
    dig_out_t    relay;
    hysteresis_t band;
    edge_count_t btn_edges;
    bool         override_active;
    bool         night;
} auto_light_app_t;

void auto_light_app_init(auto_light_app_t *app, uint8_t adc_channel,
                         uint8_t btn_pin, uint8_t relay_pin);
void auto_light_app_step(auto_light_app_t *app);
bool auto_light_output(const auto_light_app_t *app);
bool auto_light_overridden(const auto_light_app_t *app);

#endif
