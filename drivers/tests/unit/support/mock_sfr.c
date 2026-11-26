/* mock_sfr.c — backing memory + behavioral models, see xc.h. */
#include <string.h>

#include "xc.h"

static volatile uint32_t sio_out_store, sio_oe_store;
volatile uint32_t SIO_GPIO_OUT_SET, SIO_GPIO_OUT_CLR, SIO_GPIO_OUT_XOR;
volatile uint32_t SIO_GPIO_OE_SET, SIO_GPIO_OE_CLR;
volatile uint32_t SIO_GPIO_IN;

volatile uint32_t U0DR, U0FR, U0IBRD, U0FBRD, U0LCRH, U0CR;
volatile uint32_t U1DR, U1FR, U1IBRD, U1FBRD, U1LCRH, U1CR;

volatile uint32_t SPI0CR0, SPI0CR1, SPI0DR, SPI0SR, SPI0CPSR;
volatile uint32_t SPI1CR0, SPI1CR1, SPI1DR, SPI1SR, SPI1CPSR;

volatile uint32_t I2C0CON, I2C0TAR, I2C0DATACMD, I2C0ENABLE, I2C0FSHCNT, I2C0FSLCNT, I2C0STATUS;
volatile uint32_t I2C1CON, I2C1TAR, I2C1DATACMD, I2C1ENABLE, I2C1FSHCNT, I2C1FSLCNT, I2C1STATUS;

volatile uint32_t PWM_CSR[8], PWM_DIV[8], PWM_CC[8], PWM_TOP[8];

static volatile uint32_t adc_cs_store;
volatile uint32_t ADC_RESULT;

volatile uint32_t *mock_sio_out_model(void)
{
    sio_out_store |= SIO_GPIO_OUT_SET;
    sio_out_store &= ~SIO_GPIO_OUT_CLR;
    sio_out_store ^= SIO_GPIO_OUT_XOR;
    SIO_GPIO_OUT_SET = SIO_GPIO_OUT_CLR = SIO_GPIO_OUT_XOR = 0U;
    return &sio_out_store;
}

volatile uint32_t *mock_sio_oe_model(void)
{
    sio_oe_store |= SIO_GPIO_OE_SET;
    sio_oe_store &= ~SIO_GPIO_OE_CLR;
    SIO_GPIO_OE_SET = SIO_GPIO_OE_CLR = 0U;
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
    SIO_GPIO_OUT_SET = SIO_GPIO_OUT_CLR = SIO_GPIO_OUT_XOR = 0U;
    SIO_GPIO_OE_SET = SIO_GPIO_OE_CLR = 0U;
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
}
