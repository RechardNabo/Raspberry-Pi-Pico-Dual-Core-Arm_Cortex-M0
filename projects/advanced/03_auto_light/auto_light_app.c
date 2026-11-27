#include "auto_light_app.h"
#include "../../../drivers/devices/kit/kit.h"

void auto_light_app_init(auto_light_app_t *app, uint8_t adc_channel,
                         uint8_t btn_pin, uint8_t relay_pin)
{
    analog_in_config_t acfg = kit_analog_config(KIT_LDR, adc_channel);
    dig_in_config_t    bcfg = kit_dig_in_config(KIT_BUTTON, btn_pin);
    dig_out_config_t   rcfg = kit_dig_out_config(KIT_RELAY, relay_pin);
    analog_in_init(&app->sensor, &acfg);
    dig_in_init(&app->button, &bcfg);
    dig_out_init(&app->relay, &rcfg);
    hysteresis_init(&app->band, AL_DARK_COUNT, AL_BRIGHT_COUNT);
    edge_count_init(&app->btn_edges);
    app->override_active = false;
    app->night = false;
    dig_out_set(&app->relay, false);
}

void auto_light_app_step(auto_light_app_t *app)
{
    uint16_t raw = 0U;
    analog_in_read_raw(&app->sensor, &raw);
    dig_in_sample(&app->button);

    bool btn_edge = edge_count_step(&app->btn_edges, dig_in_state(&app->button));
    if (btn_edge) {
        app->override_active = !app->override_active;
    }

    bool was_night = app->night;
    (void)hysteresis_step(&app->band, (int32_t)raw);
    /* hysteresis band is inverted for light: active-high = bright */
    app->night = !hysteresis_state(&app->band);

    /* a day/night transition clears any manual override */
    if (app->night != was_night) {
        app->override_active = false;
    }

    bool output = app->night;
    if (app->override_active) {
        output = !output;
    }
    dig_out_set(&app->relay, output);
}

bool auto_light_output(const auto_light_app_t *app)
{
    bool output = app->night;
    if (app->override_active) {
        output = !output;
    }
    return output;
}

bool auto_light_overridden(const auto_light_app_t *app)
{
    return app->override_active;
}
