/* climate_guard_app.h — thermostat control + independent overheat alarm.
 * Integration: LM35 analog temperature drives TWO independent paths:
 *   control path: relay heating with hysteresis (engage below LOW, release above HIGH)
 *   alarm path:   buzzer above ALARM threshold regardless of relay state
 * The fusion contract: control and alarm must never interfere. */
#ifndef CLIMATE_GUARD_APP_H
#define CLIMATE_GUARD_APP_H

#include <stdint.h>
#include <stdbool.h>
#include "../../../drivers/devices/analog_in/analog_in.h"
#include "../../../drivers/devices/dig_out/dig_out.h"
#include "../../lib/hysteresis.h"

#define CG_HEAT_ON_COUNT    1638   /* engage heating below this raw count */
#define CG_HEAT_OFF_COUNT   2457   /* release heating above this raw count */
#define CG_ALARM_COUNT      3276   /* buzzer above this raw count */

typedef struct {
    analog_in_t sensor;
    dig_out_t   relay;
    dig_out_t   buzzer;
    bool        heating;
    bool        buzzer_on;
} climate_guard_app_t;

void climate_guard_app_init(climate_guard_app_t *app, uint8_t adc_channel,
                            uint8_t relay_pin, uint8_t buzzer_pin);
void climate_guard_app_step(climate_guard_app_t *app);
bool climate_guard_heating(const climate_guard_app_t *app);
bool climate_guard_alarm(const climate_guard_app_t *app);

#endif
