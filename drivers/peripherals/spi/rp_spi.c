/* rp_spi.c — see rp_spi.h.
 * CR0: DSS bits 3:0 (0111=8bit), SPO bit6, SPH bit7, SCR bits 15:8.
 * CR1: SSE bit1. SR: TNF bit1, RNE bit2, BSY bit4. CPSR at 0x10. */
#include <xc.h>

#include "rp_spi.h"
#include "../../bsp/rp_board.h"

#define CR0_DSS8   (7UL << 0)
#define CR0_SPO    (1UL << 6)
#define CR0_SPH    (1UL << 7)
#define CR1_SSE    (1UL << 1)
#define SR_TNF     (1UL << 1)
#define SR_RNE     (1UL << 2)

typedef struct {
    volatile uint32_t *cr0, *cr1, *dr, *sr, *cpsr;
} rp_spi_regs_t;

static rp_spi_regs_t tbl[RP_SPI_CHANNEL_COUNT];
static int tbl_ready = 0;

static const rp_spi_regs_t *regmap(rp_spi_channel_t ch)
{
    if ((unsigned int)ch >= (unsigned int)RP_SPI_CHANNEL_COUNT) return 0;
    if (!tbl_ready) {
        tbl[RP_SPI0].cr0 = &SPI0CR0; tbl[RP_SPI0].cr1 = &SPI0CR1;
        tbl[RP_SPI0].dr = &SPI0DR;  tbl[RP_SPI0].sr = &SPI0SR; tbl[RP_SPI0].cpsr = &SPI0CPSR;
        tbl[RP_SPI1].cr0 = &SPI1CR0; tbl[RP_SPI1].cr1 = &SPI1CR1;
        tbl[RP_SPI1].dr = &SPI1DR;  tbl[RP_SPI1].sr = &SPI1SR; tbl[RP_SPI1].cpsr = &SPI1CPSR;
        tbl_ready = 1;
    }
    return &tbl[ch];
}

void rp_spi_divisors(uint32_t clk_hz, uint32_t clock_hz, uint32_t *cpsdvsr, uint32_t *scr)
{
    /* smallest total division meeting target: try cpsdvsr=2 first with
     * scr as needed; scr chosen by ceil, never under-dividing */
    uint32_t best_cps = 2U, best_scr = 0U;
    uint32_t total = (clk_hz + clock_hz - 1UL) / clock_hz;  /* ceil */
    if (total < 2U) total = 2U;
    best_scr = (total + 1UL) / 2U - 1UL;   /* cps=2: total = 2*(scr+1) */
    if (best_scr > 255U) best_scr = 255U;
    *cpsdvsr = best_cps;
    *scr = best_scr;
}

rp_spi_status_t rp_spi_init(rp_spi_channel_t ch, const rp_spi_config_t *config)
{
    const rp_spi_regs_t *regs = regmap(ch);
    uint32_t cps, scr, cr0;
    if (regs == 0) return RP_SPI_STATUS_INVALID_CHANNEL;
    if (config == 0 || config->clock_hz == 0U || config->clk_hz == 0U)
        return RP_SPI_STATUS_INVALID_PARAM;
    rp_spi_divisors(config->clk_hz, config->clock_hz, &cps, &scr);

    cr0 = CR0_DSS8 | ((scr & 0xFFUL) << 8);
    /* PL022 mode mapping: mode0 SPO=0 SPH=0; mode1 SPO=0 SPH=1;
     * mode2 SPO=1 SPH=0; mode3 both — clean orthogonal encoding */
    if (config->mode == RP_SPI_MODE_1 || config->mode == RP_SPI_MODE_3) cr0 |= CR0_SPH;
    if (config->mode == RP_SPI_MODE_2 || config->mode == RP_SPI_MODE_3) cr0 |= CR0_SPO;

    *regs->cr1 = 0U;         /* disabled while configuring */
    *regs->cpsr = cps;
    *regs->cr0 = cr0;
    *regs->cr1 = CR1_SSE;    /* enable last */
    return RP_SPI_STATUS_OK;
}

rp_spi_status_t rp_spi_exchange_byte(rp_spi_channel_t ch, uint8_t tx, uint8_t *rx_out)
{
    const rp_spi_regs_t *regs = regmap(ch);
    uint32_t spin = 100000UL;
    if (regs == 0) return RP_SPI_STATUS_INVALID_CHANNEL;
    while ((*regs->sr & SR_TNF) == 0U) {
        if (--spin == 0U) return RP_SPI_STATUS_INVALID_CLOCK;
    }
    *regs->dr = tx;
    spin = 100000UL;
    while ((*regs->sr & SR_RNE) == 0U) {
        if (--spin == 0U) return RP_SPI_STATUS_INVALID_CLOCK;
    }
    if (rx_out != 0) *rx_out = (uint8_t)(*regs->dr & 0xFFU);
    return RP_SPI_STATUS_OK;
}
