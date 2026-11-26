/* rp_pwm.h — PWM driver for RP2040: 8 slices x 2 channels.
 * Free-running counter per slice; duty = CC/(TOP+1). The frequency x
 * resolution trade-off is explicit: TOP determines both. */
#ifndef RP_PWM_H
#define RP_PWM_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    RP_PWM_STATUS_OK = 0, RP_PWM_STATUS_INVALID_SLICE,
    RP_PWM_STATUS_INVALID_PARAM, RP_PWM_STATUS_FREQ_IMPOSSIBLE
} rp_pwm_status_t;

typedef struct {
    uint32_t clk_hz;
    uint32_t frequency;
    uint8_t  channel;   /* 0=A, 1=B */
} rp_pwm_config_t;

rp_pwm_status_t rp_pwm_init(uint8_t slice, const rp_pwm_config_t *config);
rp_pwm_status_t rp_pwm_set_duty_permille(uint8_t slice, uint8_t channel, uint32_t permille);
uint32_t rp_pwm_top_for_freq(uint32_t clk_hz, uint32_t frequency);

#endif
