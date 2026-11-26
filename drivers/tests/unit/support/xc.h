/* xc.h — host-test register model for RP2040 peripherals in scope.
 * Behavioral models (documented simplifications):
 *  - SIO SET/CLR/XOR aliases are flat memory; parent registers apply
 *    pending atomic writes on access (same trick as the ESP32 model).
 *  - ADC START_ONCE self-clears and sets READY on the next access —
 *    instant conversion completion.
 *  - PWM per-slice registers modeled as arrays (silicon interleaves
 *    A/B channels per slice — simplified, noted in README). */
#ifndef XC_H
#define XC_H

#include <stdint.h>
#include <stdbool.h>

/* SIO GPIO */
volatile uint32_t *mock_sio_out_model(void);
volatile uint32_t *mock_sio_oe_model(void);
#define SIO_GPIO_OUT (*mock_sio_out_model())
#define SIO_GPIO_OE  (*mock_sio_oe_model())
/* alias registers are modelled macros: every write first flushes pending
 * writes to the parent IN ORDER � preserving silicon write ordering */
volatile uint32_t *mock_out_set_w(void);
volatile uint32_t *mock_out_clr_w(void);
volatile uint32_t *mock_out_xor_w(void);
volatile uint32_t *mock_oe_set_w(void);
volatile uint32_t *mock_oe_clr_w(void);
#define SIO_GPIO_OUT_SET (*mock_out_set_w())
#define SIO_GPIO_OUT_CLR (*mock_out_clr_w())
#define SIO_GPIO_OUT_XOR (*mock_out_xor_w())
#define SIO_GPIO_OE_SET  (*mock_oe_set_w())
#define SIO_GPIO_OE_CLR  (*mock_oe_clr_w())
extern volatile uint32_t SIO_GPIO_IN;

/* UART0/1 — flat */
extern volatile uint32_t U0DR, U0FR, U0IBRD, U0FBRD, U0LCRH, U0CR;
extern volatile uint32_t U1DR, U1FR, U1IBRD, U1FBRD, U1LCRH, U1CR;

/* SPI0/1 — flat */
extern volatile uint32_t SPI0CR0, SPI0CR1, SPI0DR, SPI0SR, SPI0CPSR;
extern volatile uint32_t SPI1CR0, SPI1CR1, SPI1DR, SPI1SR, SPI1CPSR;

/* I2C0/1 — flat */
extern volatile uint32_t I2C0CON, I2C0TAR, I2C0DATACMD, I2C0ENABLE, I2C0FSHCNT, I2C0FSLCNT, I2C0STATUS;
extern volatile uint32_t I2C1CON, I2C1TAR, I2C1DATACMD, I2C1ENABLE, I2C1FSHCNT, I2C1FSLCNT, I2C1STATUS;

/* PWM — array simplification */
extern volatile uint32_t PWM_CSR[8], PWM_DIV[8], PWM_CC[8], PWM_TOP[8];

/* ADC — CS modelled, RESULT flat */
volatile uint32_t *mock_adc_cs_model(void);
#define ADC_CS (*mock_adc_cs_model())
extern volatile uint32_t ADC_RESULT;

/* timer � modelled: virtual time auto-advances per read so busy-wait
 * loops terminate; tests can schedule pin-state changes at time offsets
 * (the host equivalent of a slave device answering on the bus) */
volatile uint32_t *mock_timerawl_model(void);
volatile uint32_t *mock_timerawh_model(void);
#define TIMERAWL (*mock_timerawl_model())
#define TIMERAWH (*mock_timerawh_model())
void mock_time_reset(void);
void mock_time_schedule(uint32_t offset_us, uint8_t pin, bool level);
uint64_t mock_time_now(void);

void mock_sfr_reset(void);

#endif
