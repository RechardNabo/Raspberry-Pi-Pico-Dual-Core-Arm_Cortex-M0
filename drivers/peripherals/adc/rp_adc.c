/* rp_adc.c — see rp_adc.h.
 * CS: EN bit0, READY bit8, START_ONCE bit2, AINSEL bits 14:12.
 * RESULT bits 11:0. */
#include <xc.h>

#include "rp_adc.h"
#include "../../bsp/rp_board.h"

#define CS_EN        (1UL << 0)
#define CS_START_ONCE (1UL << 2)
#define CS_READY     (1UL << 8)

rp_adc_status_t rp_adc_init(void)
{
    ADC_CS = CS_EN;
    return RP_ADC_STATUS_OK;
}

rp_adc_status_t rp_adc_read(uint8_t channel, uint16_t *raw_out)
{
    uint32_t spin = 100000UL;
    if (!rp_adc_channel_valid(channel) || raw_out == 0) return RP_ADC_STATUS_INVALID_CHANNEL;
    ADC_CS = CS_EN | ((uint32_t)(channel & 7U) << 12) | CS_START_ONCE;
    while ((ADC_CS & CS_READY) == 0U) {
        if (--spin == 0U) return RP_ADC_STATUS_TIMEOUT;
    }
    *raw_out = (uint16_t)(ADC_RESULT & 0xFFFU);
    return RP_ADC_STATUS_OK;
}

uint16_t rp_adc_to_mv(uint16_t raw)
{
    return (uint16_t)(((uint32_t)raw * 3300UL) / 4095UL);
}
