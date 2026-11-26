/* buzzer_passive_app.h — two-octave scale player.
 * Composition: sequencer of frequencies + buzzer PWM device. */
#ifndef BUZZER_PASSIVE_APP_H
#define BUZZER_PASSIVE_APP_H

#include <stdint.h>
#include "../../drivers/devices/pwm_out/pwm_out.h"
#include "../lib/sequencer.h"

#define SCALE_NOTE_COUNT  15U

typedef struct {
    buzzer_t device;
    sequencer_t scale;
} buzzer_passive_app_t;

void buzzer_passive_app_init(buzzer_passive_app_t *app, uint8_t slice, uint8_t channel, uint32_t clk_hz);
void buzzer_passive_app_step(buzzer_passive_app_t *app);   /* one note per tick */
uint8_t buzzer_passive_app_position(const buzzer_passive_app_t *app);
const uint32_t *buzzer_passive_app_scale(void);

#endif
