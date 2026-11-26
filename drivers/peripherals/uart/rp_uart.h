/* rp_uart.h — UART driver for RP2040 (UART0/1, PL011-derived).
 * Divisor: baud = clk/(16 x (IBRD + FBRD/64)) — 6-bit fractional part.
 * Driver computes nearest integer+fraction pair and verifies the error. */
#ifndef RP_UART_H
#define RP_UART_H

#include <stdint.h>
#include <stdbool.h>

typedef enum { RP_UART0 = 0, RP_UART1, RP_UART_CHANNEL_COUNT } rp_uart_channel_t;

typedef enum {
    RP_UART_STATUS_OK = 0,
    RP_UART_STATUS_INVALID_CHANNEL,
    RP_UART_STATUS_INVALID_PARAM,
    RP_UART_STATUS_INVALID_BAUD
} rp_uart_status_t;

typedef enum { RP_UART_PARITY_NONE = 0, RP_UART_PARITY_EVEN, RP_UART_PARITY_ODD } rp_uart_parity_t;

typedef struct {
    uint32_t clk_hz;
    uint32_t baud;
    rp_uart_parity_t parity;
    uint8_t stop_bits;   /* 1 or 2 */
} rp_uart_config_t;

#define RP_UART_ERR_LIMIT_X1000  20

rp_uart_status_t rp_uart_init(rp_uart_channel_t ch, const rp_uart_config_t *config);
bool rp_uart_write_byte(rp_uart_channel_t ch, uint8_t byte);
bool rp_uart_read_byte(rp_uart_channel_t ch, uint8_t *byte_out);
void rp_uart_divisor(uint32_t clk_hz, uint32_t baud, uint32_t *ibrd, uint32_t *fbrd);

#endif
