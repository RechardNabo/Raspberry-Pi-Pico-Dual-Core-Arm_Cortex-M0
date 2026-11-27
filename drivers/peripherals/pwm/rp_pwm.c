/* rp_pwm.c — see rp_pwm.h.
 * CSR: EN bit0. DIV: INT bits 11:4, FRAC bits 3:0 (fixed 4-bit frac).
 * CC: channel A level bits 15:0, B bits 31:16. TOP bits 15:0.
 * Host model simplifies the per-slice register interleave into arrays —
 * documented in xc.h. */
#include <xc.h>

#include "rp_pwm.h"
#include "../../bsp/rp_board.h"

#define CSR_EN  (1UL << 0)

uint32_t rp_pwm_top_for_freq(uint32_t clk_hz, uint32_t frequency)
{
    /* div=1: f = clk/(top+1) — top rounded down, never over-frequency */
    if (frequency == 0U) return 0U;
    const uint32_t total = clk_hz / frequency;
    return (total == 0U) ? 0U : total - 1U;
}

rp_pwm_status_t rp_pwm_init(uint8_t slice, const rp_pwm_config_t *config)
{
    uint32_t top;
    if (slice >= 8U) return RP_PWM_STATUS_INVALID_SLICE;
    if (config == 0 || config->frequency == 0U || config->clk_hz == 0U)
        return RP_PWM_STATUS_INVALID_PARAM;
    /* TOP is 16-bit: low frequencies need the integer clock divider.
     * Smallest div bringing the period count into range � never overclocking */
    uint32_t div = 1U;
    while (div < 256U) {
        const uint32_t ticks = config->clk_hz / (div * config->frequency);
        if (ticks >= 2U && ticks <= 65536U) break;
        div++;
    }
    if (div >= 256U) return RP_PWM_STATUS_FREQ_IMPOSSIBLE;
    top = config->clk_hz / (div * config->frequency) - 1U;
    if (top > 0xFFFFUL) return RP_PWM_STATUS_FREQ_IMPOSSIBLE;

    PWM_CSR[slice * 5U] = 0U;          /* disabled while configuring */
    PWM_DIV[slice * 5U] = div << 4;    /* integer divider, frac 0 */
    PWM_TOP[slice * 5U] = top;
    PWM_CC[slice * 5U] = 0U;
    PWM_CSR[slice * 5U] = CSR_EN;      /* enable last */
    return RP_PWM_STATUS_OK;
}

rp_pwm_status_t rp_pwm_set_duty_permille(uint8_t slice, uint8_t channel, uint32_t permille)
{
    uint32_t level, cc;
    if (slice >= 8U) return RP_PWM_STATUS_INVALID_SLICE;
    if (permille > 1000U) return RP_PWM_STATUS_INVALID_PARAM;
    level = ((PWM_TOP[slice * 5U] + 1UL) * permille) / 1000U;
    cc = PWM_CC[slice * 5U];
    if (channel == 0U) {
        cc = (cc & 0xFFFF0000UL) | (level & 0xFFFFUL);
    } else {
        cc = (cc & 0xFFFFUL) | ((level & 0xFFFFUL) << 16);
    }
    PWM_CC[slice * 5U] = cc;
    return RP_PWM_STATUS_OK;
}
