/* rp_i2c.h — I2C master driver for RP2040 (I2C0/1, DW_apb_i2c).
 * Clock: FS-mode SCL high/low counts in IC clocks — the peripheral
 * does not auto-derive them; both must be programmed, and the silicon
 * adds fixed internal latency the counts must absorb. */
#ifndef RP_I2C_H
#define RP_I2C_H

#include <stdint.h>
#include <stdbool.h>

typedef enum { RP_I2C0 = 0, RP_I2C1, RP_I2C_CHANNEL_COUNT } rp_i2c_channel_t;
typedef enum {
    RP_I2C_STATUS_OK = 0, RP_I2C_STATUS_INVALID_CHANNEL,
    RP_I2C_STATUS_INVALID_PARAM, RP_I2C_STATUS_INVALID_CLOCK,
    RP_I2C_STATUS_NACK
} rp_i2c_status_t;

typedef struct { uint32_t clk_hz; uint32_t clock_hz; uint8_t slave_addr; } rp_i2c_config_t;

rp_i2c_status_t rp_i2c_init(rp_i2c_channel_t ch, const rp_i2c_config_t *config);
void rp_i2c_counts(uint32_t clk_hz, uint32_t clock_hz, uint32_t *hcnt, uint32_t *lcnt);
rp_i2c_status_t rp_i2c_write_byte(rp_i2c_channel_t ch, uint8_t byte);
rp_i2c_status_t rp_i2c_read_byte(rp_i2c_channel_t ch, uint8_t *byte_out);

#endif
