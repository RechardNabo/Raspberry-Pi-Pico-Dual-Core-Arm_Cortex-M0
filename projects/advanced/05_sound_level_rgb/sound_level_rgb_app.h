/* sound_level_rgb_app.h — sound level color meter.
 * Integration: analog microphone amplitude mapped onto the RGB LED:
 *   quiet    → blue dominant
 *   moderate → green dominant
 *   loud     → red dominant
 * Peak-hold behavior: the displayed level decays gradually after a
 * spike rather than dropping instantly — the meter is readable.
 * The fusion contract: color is a pure function of the decayed peak,
 * never the instantaneous sample. */
#ifndef SOUND_LEVEL_RGB_APP_H
#define SOUND_LEVEL_RGB_APP_H

#include <stdint.h>
#include <stdbool.h>
#include "../../../drivers/devices/analog_in/analog_in.h"
#include "../../../drivers/devices/pwm_out/pwm_out.h"

#define SL_MODERATE_COUNT  1200U
#define SL_LOUD_COUNT      2400U
#define SL_DECAY_PER_TICK  8U

typedef enum {
    SL_QUIET = 0,
    SL_MODERATE,
    SL_LOUD
} sl_band_t;

typedef struct {
    analog_in_t  mic;
    rgb_led_t    rgb;
    uint32_t     peak;
    sl_band_t    band;
} sound_level_rgb_app_t;

void sound_level_rgb_app_init(sound_level_rgb_app_t *app, uint8_t adc_channel,
                              uint8_t r_pin, uint8_t g_pin, uint8_t b_pin);
void sound_level_rgb_app_step(sound_level_rgb_app_t *app);
sl_band_t sound_level_rgb_band(const sound_level_rgb_app_t *app);
uint32_t sound_level_rgb_peak(const sound_level_rgb_app_t *app);

#endif
