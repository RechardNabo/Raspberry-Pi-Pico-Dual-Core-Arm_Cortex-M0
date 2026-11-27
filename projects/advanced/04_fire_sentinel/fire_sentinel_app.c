#include "fire_sentinel_app.h"
#include "../../../drivers/devices/kit/kit.h"

void fire_sentinel_app_init(fire_sentinel_app_t *app, uint8_t flame_pin,
                            uint8_t adc_channel, uint8_t buzzer_pin, uint8_t led_pin)
{
    dig_in_config_t    fcfg = kit_dig_in_config(KIT_FLAME, flame_pin);
    analog_in_config_t acfg = kit_analog_config(KIT_LM35, adc_channel);
    dig_out_config_t   zcfg = kit_dig_out_config(KIT_BUZZER_ACTIVE, buzzer_pin);
    dig_out_config_t   lcfg = kit_dig_out_config(KIT_DUAL_LED, led_pin);
    dig_in_init(&app->flame, &fcfg);
    analog_in_init(&app->temp, &acfg);
    dig_out_init(&app->buzzer, &zcfg);
    dig_out_init(&app->led, &lcfg);
    app->level = FS_CLEAR;
    dig_out_set(&app->buzzer, false);
    dig_out_set(&app->led, false);
}

void fire_sentinel_app_step(fire_sentinel_app_t *app)
{
    dig_in_sample(&app->flame);
    bool flame = dig_in_state(&app->flame);
    uint16_t raw = 0U;
    analog_in_read_raw(&app->temp, &raw);
    bool hot = raw > FS_HOT_COUNT;

    if (flame && hot) {
        app->level = FS_ALARM;
    } else if (flame || hot) {
        app->level = FS_WARNING;
    } else {
        app->level = FS_CLEAR;
    }

    dig_out_set(&app->led, app->level != FS_CLEAR);
    dig_out_set(&app->buzzer, app->level == FS_ALARM);
}

fs_level_t fire_sentinel_level(const fire_sentinel_app_t *app)
{
    return app->level;
}
