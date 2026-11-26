/* rp_gpio.c — SIO GPIO_OE/OUT with atomic aliases; GPIO_IN for reads. */
#include <xc.h>

#include "rp_gpio.h"
#include "../../bsp/rp_board.h"

rp_gpio_status_t rp_gpio_set_output(uint8_t pin)
{
    if (!rp_gpio_valid(pin)) return RP_GPIO_STATUS_INVALID_PIN;
    SIO_GPIO_OE_SET = 1UL << pin;
    return RP_GPIO_STATUS_OK;
}

rp_gpio_status_t rp_gpio_set_input(uint8_t pin)
{
    if (!rp_gpio_valid(pin)) return RP_GPIO_STATUS_INVALID_PIN;
    SIO_GPIO_OE_CLR = 1UL << pin;
    return RP_GPIO_STATUS_OK;
}

rp_gpio_status_t rp_gpio_write(uint8_t pin, bool high)
{
    if (!rp_gpio_valid(pin)) return RP_GPIO_STATUS_INVALID_PIN;
    if (high) SIO_GPIO_OUT_SET = 1UL << pin;
    else      SIO_GPIO_OUT_CLR = 1UL << pin;
    return RP_GPIO_STATUS_OK;
}

rp_gpio_status_t rp_gpio_toggle(uint8_t pin)
{
    if (!rp_gpio_valid(pin)) return RP_GPIO_STATUS_INVALID_PIN;
    SIO_GPIO_OUT_XOR = 1UL << pin;
    return RP_GPIO_STATUS_OK;
}

bool rp_gpio_read(uint8_t pin)
{
    if (!rp_gpio_valid(pin)) return false;
    return (SIO_GPIO_IN & (1UL << pin)) != 0U;
}
