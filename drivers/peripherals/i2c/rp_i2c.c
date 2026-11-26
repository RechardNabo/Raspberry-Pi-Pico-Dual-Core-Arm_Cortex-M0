/* rp_i2c.c — see rp_i2c.h.
 * CON: MASTER bit0, SPEED bits 2:1 (10b=fast), RESTART_EN bit5.
 * ENABLE bit0 at IC_ENABLE. TAR at IC_TAR bits 6:0 (7-bit mode).
 * DATA_CMD: byte bits 7:0, CMD bit8 (1=read). STATUS: TFNF bit1 RFNE bit2. */
#include <xc.h>

#include "rp_i2c.h"
#include "../../bsp/rp_board.h"

#define CON_MASTER    (1UL << 0)
#define CON_SPEED_FS  (2UL << 1)
#define CON_RESTART   (1UL << 5)
#define STAT_TFNF     (1UL << 1)
#define STAT_RFNE     (1UL << 2)
#define DATACMD_READ  (1UL << 8)

typedef struct {
    volatile uint32_t *con, *tar, *data_cmd, *enable, *hcnt, *lcnt, *status;
} rp_i2c_regs_t;

static rp_i2c_regs_t tbl[RP_I2C_CHANNEL_COUNT];
static int tbl_ready = 0;

static const rp_i2c_regs_t *regmap(rp_i2c_channel_t ch)
{
    if ((unsigned int)ch >= (unsigned int)RP_I2C_CHANNEL_COUNT) return 0;
    if (!tbl_ready) {
        tbl[RP_I2C0].con = &I2C0CON; tbl[RP_I2C0].tar = &I2C0TAR;
        tbl[RP_I2C0].data_cmd = &I2C0DATACMD; tbl[RP_I2C0].enable = &I2C0ENABLE;
        tbl[RP_I2C0].hcnt = &I2C0FSHCNT; tbl[RP_I2C0].lcnt = &I2C0FSLCNT;
        tbl[RP_I2C0].status = &I2C0STATUS;
        tbl[RP_I2C1].con = &I2C1CON; tbl[RP_I2C1].tar = &I2C1TAR;
        tbl[RP_I2C1].data_cmd = &I2C1DATACMD; tbl[RP_I2C1].enable = &I2C1ENABLE;
        tbl[RP_I2C1].hcnt = &I2C1FSHCNT; tbl[RP_I2C1].lcnt = &I2C1FSLCNT;
        tbl[RP_I2C1].status = &I2C1STATUS;
        tbl_ready = 1;
    }
    return &tbl[ch];
}

void rp_i2c_counts(uint32_t clk_hz, uint32_t clock_hz, uint32_t *hcnt, uint32_t *lcnt)
{
    /* DW internal latency: ~6 IC clocks on high, ~8 on low — counts must
     * cover them; total split after absorbing latency */
    const uint32_t total = clk_hz / clock_hz;
    uint32_t h = total / 2U, l = total - h;
    if (h > 7U) h -= 7U; else h = 1U;
    if (l > 9U) l -= 9U; else l = 1U;
    *hcnt = h;
    *lcnt = l;
}

rp_i2c_status_t rp_i2c_init(rp_i2c_channel_t ch, const rp_i2c_config_t *config)
{
    const rp_i2c_regs_t *regs = regmap(ch);
    uint32_t h, l;
    if (regs == 0) return RP_I2C_STATUS_INVALID_CHANNEL;
    if (config == 0 || config->clock_hz == 0U || config->clk_hz == 0U)
        return RP_I2C_STATUS_INVALID_PARAM;
    if (config->slave_addr > 0x7FU) return RP_I2C_STATUS_INVALID_PARAM;

    *regs->enable = 0U;   /* mandatory: counts only writable while disabled */
    rp_i2c_counts(config->clk_hz, config->clock_hz, &h, &l);
    if (h == 0U || l == 0U) return RP_I2C_STATUS_INVALID_CLOCK;
    *regs->hcnt = h;
    *regs->lcnt = l;
    *regs->tar = config->slave_addr;
    *regs->con = CON_MASTER | CON_SPEED_FS | CON_RESTART;
    *regs->enable = 1U;   /* enable last, after all config */
    return RP_I2C_STATUS_OK;
}

rp_i2c_status_t rp_i2c_write_byte(rp_i2c_channel_t ch, uint8_t byte)
{
    const rp_i2c_regs_t *regs = regmap(ch);
    if (regs == 0) return RP_I2C_STATUS_INVALID_CHANNEL;
    if ((*regs->status & STAT_TFNF) == 0U) return RP_I2C_STATUS_NACK;
    *regs->data_cmd = byte;   /* cmd bit clear = write */
    return RP_I2C_STATUS_OK;
}

rp_i2c_status_t rp_i2c_read_byte(rp_i2c_channel_t ch, uint8_t *byte_out)
{
    const rp_i2c_regs_t *regs = regmap(ch);
    if (regs == 0 || byte_out == 0) return RP_I2C_STATUS_INVALID_CHANNEL;
    if ((*regs->status & STAT_RFNE) == 0U) return RP_I2C_STATUS_NACK;
    *byte_out = (uint8_t)(*regs->data_cmd & 0xFFU);
    return RP_I2C_STATUS_OK;
}
