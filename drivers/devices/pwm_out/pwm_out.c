/* pwm_out.c — see pwm_out.h. */
#include "pwm_out.h"
#include "../../peripherals/pwm/rp_pwm.h"

pwm_out_status_t pwm_out_init_channel(pwm_out_channel_t *ch, uint8_t slice, uint8_t channel,
                                      uint32_t clk_hz, uint32_t frequency)
{
    rp_pwm_config_t cfg;
    if (ch == 0 || slice >= 8U || channel > 1U) return PWM_OUT_STATUS_INVALID_CONFIG;
    cfg.clk_hz = clk_hz;
    cfg.frequency = frequency;
    cfg.channel = channel;
    if (rp_pwm_init(slice, &cfg) != RP_PWM_STATUS_OK) return PWM_OUT_STATUS_INVALID_CONFIG;
    ch->slice = slice;
    ch->channel = channel;
    return PWM_OUT_STATUS_OK;
}

pwm_out_status_t pwm_out_set_permille(const pwm_out_channel_t *ch, uint32_t permille)
{
    if (ch == 0 || permille > 1000U) return PWM_OUT_STATUS_INVALID_PARAM;
    if (rp_pwm_set_duty_permille(ch->slice, ch->channel, permille) != RP_PWM_STATUS_OK)
        return PWM_OUT_STATUS_INVALID_CONFIG;
    return PWM_OUT_STATUS_OK;
}

pwm_out_status_t rgb_led_init(rgb_led_t *dev, uint8_t r_slice, uint8_t r_chan,
                              uint8_t g_slice, uint8_t g_chan,
                              uint8_t b_slice, uint8_t b_chan,
                              uint32_t clk_hz, uint32_t frequency)
{
    if (dev == 0) return PWM_OUT_STATUS_INVALID_CONFIG;
    if (pwm_out_init_channel(&dev->r, r_slice, r_chan, clk_hz, frequency) != PWM_OUT_STATUS_OK)
        return PWM_OUT_STATUS_INVALID_CONFIG;
    /* slices shared between channels only need init once — rp_pwm_init is
     * idempotent for the slice, so uniform init is safe and simple */
    if (pwm_out_init_channel(&dev->g, g_slice, g_chan, clk_hz, frequency) != PWM_OUT_STATUS_OK)
        return PWM_OUT_STATUS_INVALID_CONFIG;
    if (pwm_out_init_channel(&dev->b, b_slice, b_chan, clk_hz, frequency) != PWM_OUT_STATUS_OK)
        return PWM_OUT_STATUS_INVALID_CONFIG;
    return rgb_led_off(dev);
}

pwm_out_status_t rgb_led_set(rgb_led_t *dev, uint8_t red, uint8_t green, uint8_t blue)
{
    /* 0-255 component -> permille: x*1000/255 in integer math */
    if (dev == 0) return PWM_OUT_STATUS_INVALID_CONFIG;
    pwm_out_set_permille(&dev->r, ((uint32_t)red * 1000UL) / 255UL);
    pwm_out_set_permille(&dev->g, ((uint32_t)green * 1000UL) / 255UL);
    pwm_out_set_permille(&dev->b, ((uint32_t)blue * 1000UL) / 255UL);
    return PWM_OUT_STATUS_OK;
}

pwm_out_status_t rgb_led_off(rgb_led_t *dev)
{
    return rgb_led_set(dev, 0U, 0U, 0U);
}

pwm_out_status_t buzzer_init(buzzer_t *dev, uint8_t slice, uint8_t channel, uint32_t clk_hz)
{
    rp_pwm_config_t cfg;
    if (dev == 0 || slice >= 8U || channel > 1U) return PWM_OUT_STATUS_INVALID_CONFIG;
    cfg.clk_hz = clk_hz;
    cfg.frequency = 2000UL;   /* idle frequency — silent because duty 0 */
    cfg.channel = channel;
    if (rp_pwm_init(slice, &cfg) != RP_PWM_STATUS_OK) return PWM_OUT_STATUS_INVALID_CONFIG;
    dev->slice = slice;
    dev->channel = channel;
    dev->clk_hz = clk_hz;
    dev->sounding = false;
    rp_pwm_set_duty_permille(slice, channel, 0U);
    return PWM_OUT_STATUS_OK;
}

pwm_out_status_t buzzer_tone(buzzer_t *dev, uint32_t frequency)
{
    rp_pwm_config_t cfg;
    if (dev == 0 || frequency == 0U) return PWM_OUT_STATUS_INVALID_PARAM;
    /* retune the slice: frequency changes TOP, so reconfigure — then 50%
     * duty for maximum volume at the resonant transducer */
    cfg.clk_hz = dev->clk_hz;
    cfg.frequency = frequency;
    cfg.channel = dev->channel;
    if (rp_pwm_init(dev->slice, &cfg) != RP_PWM_STATUS_OK)
        return PWM_OUT_STATUS_INVALID_PARAM;
    rp_pwm_set_duty_permille(dev->slice, dev->channel, 500U);
    dev->sounding = true;
    return PWM_OUT_STATUS_OK;
}

pwm_out_status_t buzzer_off(buzzer_t *dev)
{
    if (dev == 0) return PWM_OUT_STATUS_INVALID_CONFIG;
    rp_pwm_set_duty_permille(dev->slice, dev->channel, 0U);
    dev->sounding = false;
    return PWM_OUT_STATUS_OK;
}
