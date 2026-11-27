/* mock_sfr.c â€” backing memory + behavioral models, see xc.h. */
#include <string.h>
#include <stdbool.h>

#include "xc.h"

void mock_time_reset(void);

static volatile uint32_t sio_out_store, sio_oe_store;
static volatile uint32_t out_set_cell, out_clr_cell, out_xor_cell;
static volatile uint32_t oe_set_cell, oe_clr_cell;
volatile uint32_t SIO_GPIO_IN;

volatile uint32_t U0DR, U0FR, U0IBRD, U0FBRD, U0LCRH, U0CR;
volatile uint32_t U1DR, U1FR, U1IBRD, U1FBRD, U1LCRH, U1CR;

volatile uint32_t SPI0CR0, SPI0CR1, SPI0DR, SPI0SR, SPI0CPSR;
volatile uint32_t SPI1CR0, SPI1CR1, SPI1DR, SPI1SR, SPI1CPSR;

volatile uint32_t I2C0CON, I2C0TAR, I2C0DATACMD, I2C0ENABLE, I2C0FSHCNT, I2C0FSLCNT, I2C0STATUS;
volatile uint32_t I2C1CON, I2C1TAR, I2C1DATACMD, I2C1ENABLE, I2C1FSHCNT, I2C1FSLCNT, I2C1STATUS;

volatile uint32_t PWM_CSR[40], PWM_DIV[40], PWM_CC[40], PWM_TOP[40];

static volatile uint32_t adc_cs_store;
volatile uint32_t ADC_RESULT;

static void flush_out(void)
{
    sio_out_store &= ~out_clr_cell;
    sio_out_store |= out_set_cell;
    sio_out_store ^= out_xor_cell;
    out_clr_cell = out_set_cell = out_xor_cell = 0U;
}

static void flush_oe(void)
{
    sio_oe_store &= ~oe_clr_cell;
    sio_oe_store |= oe_set_cell;
    oe_clr_cell = oe_set_cell = 0U;
}

volatile uint32_t *mock_out_set_w(void) { flush_out(); return &out_set_cell; }
volatile uint32_t *mock_out_clr_w(void) { flush_out(); return &out_clr_cell; }
volatile uint32_t *mock_out_xor_w(void) { flush_out(); return &out_xor_cell; }
volatile uint32_t *mock_oe_set_w(void)  { flush_oe();  return &oe_set_cell; }
volatile uint32_t *mock_oe_clr_w(void)  { flush_oe();  return &oe_clr_cell; }

volatile uint32_t *mock_sio_out_model(void)
{
    flush_out();
    return &sio_out_store;
}

volatile uint32_t *mock_sio_oe_model(void)
{
    flush_oe();
    return &sio_oe_store;
}

volatile uint32_t *mock_adc_cs_model(void)
{
    if ((adc_cs_store & (1UL << 2)) != 0U) {
        adc_cs_store &= ~(1UL << 2);   /* START_ONCE consumed */
        adc_cs_store |= (1UL << 8);    /* conversion complete: READY */
    }
    return &adc_cs_store;
}

void mock_sfr_reset(void)
{
    sio_out_store = sio_oe_store = 0U;
    out_set_cell = out_clr_cell = out_xor_cell = 0U;
    oe_set_cell = oe_clr_cell = 0U;
    SIO_GPIO_IN = 0U;
    U0DR = U0FR = U0IBRD = U0FBRD = U0LCRH = U0CR = 0U;
    U1DR = U1FR = U1IBRD = U1FBRD = U1LCRH = U1CR = 0U;
    SPI0CR0 = SPI0CR1 = SPI0DR = SPI0SR = SPI0CPSR = 0U;
    SPI1CR0 = SPI1CR1 = SPI1DR = SPI1SR = SPI1CPSR = 0U;
    I2C0CON = I2C0TAR = I2C0DATACMD = I2C0ENABLE = I2C0FSHCNT = I2C0FSLCNT = I2C0STATUS = 0U;
    I2C1CON = I2C1TAR = I2C1DATACMD = I2C1ENABLE = I2C1FSHCNT = I2C1FSLCNT = I2C1STATUS = 0U;
    memset((void *)PWM_CSR, 0, sizeof(PWM_CSR));
    memset((void *)PWM_DIV, 0, sizeof(PWM_DIV));
    memset((void *)PWM_CC, 0, sizeof(PWM_CC));
    memset((void *)PWM_TOP, 0, sizeof(PWM_TOP));
    adc_cs_store = 0U;
    ADC_RESULT = 0U;
    mock_time_reset();
}

/* ---- virtual time engine ----
 * Each timer read advances virtual time by the quantum — busy-wait loops
 * in bitbang drivers terminate, and the total virtual time a driver
 * consumes is measurable evidence of the timing it requested. */
#define MOCK_TIME_QUANTUM_US  2ULL
#define MOCK_SCHED_MAX        8

static uint64_t vtime_us;
static volatile uint32_t timerawl_cell, timerawh_cell;
static struct {
    uint64_t at_us;
    uint8_t pin;
    bool level;
} sched[MOCK_SCHED_MAX];
static int sched_count;

void mock_time_reset(void)
{
    int i;
    vtime_us = 0ULL;
    sched_count = 0;
    for (i = 0; i < MOCK_SCHED_MAX; i++) {
        sched[i].at_us = 0ULL;
        sched[i].pin = 0U;
        sched[i].level = false;
    }
}

uint64_t mock_time_now(void) { return vtime_us; }

void mock_time_schedule(uint32_t offset_us, uint8_t pin, bool level)
{
    if (sched_count >= MOCK_SCHED_MAX) return;
    sched[sched_count].at_us = vtime_us + offset_us;
    sched[sched_count].pin = pin;
    sched[sched_count].level = level;
    sched_count++;
}

static void time_tick(void)
{
    int i;
    vtime_us += MOCK_TIME_QUANTUM_US;
    for (i = 0; i < sched_count; i++) {
        if (sched[i].at_us != 0ULL && vtime_us >= sched[i].at_us) {
            if (sched[i].level) SIO_GPIO_IN |= (1UL << sched[i].pin);
            else                SIO_GPIO_IN &= ~(1UL << sched[i].pin);
            sched[i].at_us = 0ULL;   /* consumed */
        }
    }
}

volatile uint32_t *mock_timerawl_model(void)
{
    time_tick();
    timerawl_cell = (uint32_t)(vtime_us & 0xFFFFFFFFULL);
    return &timerawl_cell;
}

volatile uint32_t *mock_timerawh_model(void)
{
    timerawh_cell = (uint32_t)(vtime_us >> 32);
    return &timerawh_cell;
}
