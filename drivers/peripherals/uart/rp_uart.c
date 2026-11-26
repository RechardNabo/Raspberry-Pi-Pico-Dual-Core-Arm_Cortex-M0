/* rp_uart.c — see rp_uart.h.
 * FR: TXFF bit5, RXFE bit4. LCR_H: WLEN bits 6:5 (11=8), FEN bit4,
 * STP2 bit3, EPS bit2, PEN bit1. CR: UARTEN bit0, TXE bit8, RXE bit9. */
#include <xc.h>

#include "rp_uart.h"
#include "../../bsp/rp_board.h"

#define FR_RXFE   (1UL << 4)
#define FR_TXFF   (1UL << 5)
#define LCRH_PEN  (1UL << 1)
#define LCRH_EPS  (1UL << 2)
#define LCRH_STP2 (1UL << 3)
#define LCRH_FEN  (1UL << 4)
#define LCRH_WLEN8 (3UL << 5)
#define CR_UARTEN (1UL << 0)
#define CR_TXE    (1UL << 8)
#define CR_RXE    (1UL << 9)

typedef struct {
    volatile uint32_t *dr, *fr, *ibrd, *fbrd, *lcrh, *cr;
} rp_uart_regs_t;

static rp_uart_regs_t tbl[RP_UART_CHANNEL_COUNT];
static int tbl_ready = 0;

static const rp_uart_regs_t *regmap(rp_uart_channel_t ch)
{
    if ((unsigned int)ch >= (unsigned int)RP_UART_CHANNEL_COUNT) return 0;
    if (!tbl_ready) {
        tbl[RP_UART0].dr = &U0DR;  tbl[RP_UART0].fr = &U0FR;
        tbl[RP_UART0].ibrd = &U0IBRD; tbl[RP_UART0].fbrd = &U0FBRD;
        tbl[RP_UART0].lcrh = &U0LCRH; tbl[RP_UART0].cr = &U0CR;
        tbl[RP_UART1].dr = &U1DR;  tbl[RP_UART1].fr = &U1FR;
        tbl[RP_UART1].ibrd = &U1IBRD; tbl[RP_UART1].fbrd = &U1FBRD;
        tbl[RP_UART1].lcrh = &U1LCRH; tbl[RP_UART1].cr = &U1CR;
        tbl_ready = 1;
    }
    return &tbl[ch];
}

void rp_uart_divisor(uint32_t clk_hz, uint32_t baud, uint32_t *ibrd, uint32_t *fbrd)
{
    /* divisor in 64ths: clk/(16*baud)*64 = clk*4/baud, rounded */
    const uint64_t d64 = ((uint64_t)clk_hz * 4UL + baud / 2U) / baud;
    *ibrd = (uint32_t)(d64 / 64U);
    *fbrd = (uint32_t)(d64 % 64U);
}

rp_uart_status_t rp_uart_init(rp_uart_channel_t ch, const rp_uart_config_t *config)
{
    const rp_uart_regs_t *regs = regmap(ch);
    uint32_t ibrd, fbrd, lcrh;
    uint64_t actual_x1000, err_x1000;

    if (regs == 0) return RP_UART_STATUS_INVALID_CHANNEL;
    if (config == 0 || config->baud == 0U || config->clk_hz == 0U)
        return RP_UART_STATUS_INVALID_PARAM;

    rp_uart_divisor(config->clk_hz, config->baud, &ibrd, &fbrd);
    if (ibrd == 0U || ibrd > 0xFFFFUL) return RP_UART_STATUS_INVALID_BAUD;

    actual_x1000 = ((uint64_t)config->clk_hz * 1000UL * 64UL)
                 / (16UL * ((uint64_t)ibrd * 64UL + fbrd));
    err_x1000 = (actual_x1000 > config->baud * 1000UL)
        ? actual_x1000 - config->baud * 1000UL : config->baud * 1000UL - actual_x1000;
    if ((err_x1000 / config->baud) > RP_UART_ERR_LIMIT_X1000)
        return RP_UART_STATUS_INVALID_BAUD;

    /* LCR_H must be written with the UART disabled; order matters */
    *regs->cr = 0U;
    lcrh = LCRH_WLEN8 | LCRH_FEN;
    if (config->parity != RP_UART_PARITY_NONE) {
        lcrh |= LCRH_PEN;
        if (config->parity == RP_UART_PARITY_EVEN) lcrh |= LCRH_EPS;
    }
    if (config->stop_bits == 2U) lcrh |= LCRH_STP2;
    *regs->lcrh = lcrh;
    *regs->ibrd = ibrd;
    *regs->fbrd = fbrd;
    *regs->cr = CR_UARTEN | CR_TXE | CR_RXE;   /* enable last */
    return RP_UART_STATUS_OK;
}

bool rp_uart_write_byte(rp_uart_channel_t ch, uint8_t byte)
{
    const rp_uart_regs_t *regs = regmap(ch);
    if (regs == 0) return false;
    if ((*regs->fr & FR_TXFF) != 0U) return false;
    *regs->dr = byte;
    return true;
}

bool rp_uart_read_byte(rp_uart_channel_t ch, uint8_t *byte_out)
{
    const rp_uart_regs_t *regs = regmap(ch);
    if (regs == 0 || byte_out == 0) return false;
    if ((*regs->fr & FR_RXFE) != 0U) return false;
    *byte_out = (uint8_t)(*regs->dr & 0xFFU);
    return true;
}
