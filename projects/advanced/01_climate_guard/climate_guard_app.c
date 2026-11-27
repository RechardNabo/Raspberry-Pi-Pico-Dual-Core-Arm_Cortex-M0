#include "climate_guard_app.h"
#include "../../../drivers/devices/kit/kit.h"

void climate_guard_app_init(climate_guard_app_t *app, uint8_t adc_channel,
                            uint8_t relay_pin, uint8_t buzzer_pin)
{
    analog_in_config_t acfg = kit_analog_config(KIT_LM35, adc_channel);
    dig_out_config_t   rcfg = kit_dig_out_config(KIT_RELAY, relay_pin);
    dig_out_config_t   bcfg = kit_dig_out_config(KIT_BUZZER_ACTIVE, buzzer_pin);
    analog_in_init(&app->sensor, &acfg);
    dig_out_init(&app->relay, &rcfg);
    dig_out_init(&app->buzzer, &bcfg);
    app->heating = false;
    app->buzzer_on = false;
    dig_out_set(&app->relay, false);
    dig_out_set(&app->buzzer, false);
}

void climate_guard_app_step(climate_guard_app_t *app)
{
    uint16_t raw = 0U;
    analog_in_read_raw(&app->sensor, &raw);

    /* control path — heating engages BELOW the low count and releases
     * ABOVE the high count (inverse polarity of the hysteresis primitive,
     * so the band logic is explicit here), deadband holds state */
    if ((int32_t)raw < CG_HEAT_ON_COUNT) {
        app->heating = true;
    } else if ((int32_t)raw > CG_HEAT_OFF_COUNT) {
        app->heating = false;
    }
    dig_out_set(&app->relay, app->heating);

    /* alarm path — single threshold, fully independent of the relay */
    app->buzzer_on = (raw > CG_ALARM_COUNT);
    dig_out_set(&app->buzzer, app->buzzer_on);
}

bool climate_guard_heating(const climate_guard_app_t *app)
{
    return app->heating;
}

bool climate_guard_alarm(const climate_guard_app_t *app)
{
    return app->buzzer_on;
}
