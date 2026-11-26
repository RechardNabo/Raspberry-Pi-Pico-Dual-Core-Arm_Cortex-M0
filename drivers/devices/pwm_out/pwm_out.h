/* pwm_out.h — PWM-output device class: RGB LED (#12) and passive buzzer
 * (#18). RGB is three PWM channels mixed per-component; the buzzer is a
 * single channel whose FREQUENCY is the parameter — unlike the LED, tone
 * generation reconfigures the slice TOP, not just the duty. */
#ifndef PWM_OUT_H
#define PWM_OUT_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    PWM_OUT_STATUS_OK = 0,
    PWM_OUT_STATUS_INVALID_CONFIG,
    PWM_OUT_STATUS_INVALID_PARAM
} pwm_out_status_t;

/* ---- single-channel primitive ---- */

typedef struct {
    uint8_t slice;
    uint8_t channel;   /* 0=A, 1=B */
} pwm_out_channel_t;

pwm_out_status_t pwm_out_init_channel(pwm_out_channel_t *ch, uint8_t slice, uint8_t channel,
                                      uint32_t clk_hz, uint32_t frequency);
pwm_out_status_t pwm_out_set_permille(const pwm_out_channel_t *ch, uint32_t permille);

/* ---- RGB LED (#12): common-cathode, three channels ---- */

typedef struct {
    pwm_out_channel_t r, g, b;
} rgb_led_t;

pwm_out_status_t rgb_led_init(rgb_led_t *dev, uint8_t r_slice, uint8_t r_chan,
                              uint8_t g_slice, uint8_t g_chan,
                              uint8_t b_slice, uint8_t b_chan,
                              uint32_t clk_hz, uint32_t frequency);
pwm_out_status_t rgb_led_set(rgb_led_t *dev, uint8_t red, uint8_t green, uint8_t blue);
pwm_out_status_t rgb_led_off(rgb_led_t *dev);

/* ---- passive buzzer (#18): frequency IS the note ---- */

typedef struct {
    uint8_t slice;
    uint8_t channel;
    uint32_t clk_hz;
    bool sounding;
} buzzer_t;

pwm_out_status_t buzzer_init(buzzer_t *dev, uint8_t slice, uint8_t channel, uint32_t clk_hz);
pwm_out_status_t buzzer_tone(buzzer_t *dev, uint32_t frequency);
pwm_out_status_t buzzer_off(buzzer_t *dev);

#endif
