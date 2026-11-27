#include "sound_level_rgb_app.h"
#include "../../../drivers/devices/kit/kit.h"

void sound_level_rgb_app_init(sound_level_rgb_app_t *app, uint8_t adc_channel,
                              uint8_t r_pin, uint8_t g_pin, uint8_t b_pin)
{
    analog_in_config_t acfg = kit_analog_config(KIT_MIC_SOUND, adc_channel);
    analog_in_init(&app->mic, &acfg);
    rgb_led_init(&app->rgb, r_pin, 0U, g_pin, 0U, b_pin, 0U, 125000000U, 1000U);
    app->peak = 0U;
    app->band = SL_QUIET;
    rgb_led_off(&app->rgb);
}

static sl_band_t classify(uint32_t level)
{
    if (level >= SL_LOUD_COUNT) return SL_LOUD;
    if (level >= SL_MODERATE_COUNT) return SL_MODERATE;
    return SL_QUIET;
}

void sound_level_rgb_app_step(sound_level_rgb_app_t *app)
{
    uint16_t raw = 0U;
    analog_in_read_raw(&app->mic, &raw);

    /* peak hold with linear decay */
    if (raw > app->peak) {
        app->peak = raw;
    } else if (app->peak > SL_DECAY_PER_TICK) {
        app->peak -= SL_DECAY_PER_TICK;
    } else {
        app->peak = 0U;
    }

    app->band = classify(app->peak);

    switch (app->band) {
    case SL_LOUD:     rgb_led_set(&app->rgb, 255U, 0U, 0U);   break;
    case SL_MODERATE: rgb_led_set(&app->rgb, 0U, 255U, 0U);   break;
    default:          rgb_led_set(&app->rgb, 0U, 0U, 255U);   break;
    }
}

sl_band_t sound_level_rgb_band(const sound_level_rgb_app_t *app)
{
    return app->band;
}

uint32_t sound_level_rgb_peak(const sound_level_rgb_app_t *app)
{
    return app->peak;
}
