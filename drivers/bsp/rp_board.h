/* rp_board.h — board facts, Raspberry Pi Pico (RP2040).
 * 32-bit registers. The SIO GPIO block carries atomic SET/CLR/XOR aliases —
 * the correct idiom on a dual-core chip. clk_sys/clk_peri 125 MHz default. */
#ifndef RP_BOARD_H
#define RP_BOARD_H

#include <stdint.h>
#include <stdbool.h>

#define RP_CLK_SYS_HZ   125000000UL
#define RP_CLK_PERI_HZ  125000000UL

/* Pico user GPIO 0-29; pin 25 is the onboard LED; pins 23/24/29 carry
 * board functions (SMPS/VSYS/VREF) — usable but flagged */
static inline bool rp_gpio_valid(uint8_t pin) { return pin <= 29U; }
static inline bool rp_gpio_board_function(uint8_t pin)
{
    return pin == 23U || pin == 24U || pin == 29U;
}

#define RP_UART_PRESENT_MASK  0x3U   /* UART0, UART1 */
#define RP_SPI_PRESENT_MASK   0x3U
#define RP_I2C_PRESENT_MASK   0x3U

/* ADC: GPIO26-29 carry ADC0-3; ADC4 is the internal temperature sensor */
static inline bool rp_adc_channel_valid(uint8_t ch) { return ch <= 4U; }
static inline int8_t rp_adc_gpio_for_channel(uint8_t ch)
{
    return (ch <= 3U) ? (int8_t)(26 + ch) : -1;
}

#endif
