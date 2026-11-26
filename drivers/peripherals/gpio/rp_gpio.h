/* rp_gpio.h — GPIO driver for RP2040 via the SIO block.
 * Atomic SET/CLR/XOR aliases throughout — no RMW races across the cores. */
#ifndef RP_GPIO_H
#define RP_GPIO_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    RP_GPIO_STATUS_OK = 0,
    RP_GPIO_STATUS_INVALID_PIN
} rp_gpio_status_t;

rp_gpio_status_t rp_gpio_set_output(uint8_t pin);
rp_gpio_status_t rp_gpio_set_input(uint8_t pin);
rp_gpio_status_t rp_gpio_write(uint8_t pin, bool high);
rp_gpio_status_t rp_gpio_toggle(uint8_t pin);
bool rp_gpio_read(uint8_t pin);

#endif
