/* rp_adc.h — ADC driver for RP2040: 5 inputs (GPIO26-29 + internal temp).
 * One-shot conversion model: select input, START_ONCE, poll READY.
 * 12-bit result, 3.3V reference on the Pico board. */
#ifndef RP_ADC_H
#define RP_ADC_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    RP_ADC_STATUS_OK = 0,
    RP_ADC_STATUS_INVALID_CHANNEL,
    RP_ADC_STATUS_TIMEOUT
} rp_adc_status_t;

rp_adc_status_t rp_adc_init(void);
rp_adc_status_t rp_adc_read(uint8_t channel, uint16_t *raw_out);
/* millivolts conversion — integer math, 3.3V ref */
uint16_t rp_adc_to_mv(uint16_t raw);

#endif
