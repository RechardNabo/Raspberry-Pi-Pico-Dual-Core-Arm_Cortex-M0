/* test_rp_i2c.c — DW latency absorption + enable-last order, bit-exact. */
#include "unity.h"
#include "xc.h"
#include "../../peripherals/i2c/rp_i2c.h"

void setUp(void) { mock_sfr_reset(); }
void tearDown(void) {}

static rp_i2c_config_t cfg400k(void)
{
    rp_i2c_config_t c;
    c.clk_hz = 125000000UL;
    c.clock_hz = 400000UL;
    c.slave_addr = 0x50U;
    return c;
}

static void test_counts_absorb_internal_latency(void)
{
    uint32_t h, l;
    rp_i2c_counts(125000000UL, 400000UL, &h, &l);
    /* total 312, split 156/156, minus latencies 7/9 */
    TEST_ASSERT_EQUAL_UINT32(149U, h);
    TEST_ASSERT_EQUAL_UINT32(147U, l);
}

static void test_init_programs_counts_tar_con_enable_last(void)
{
    rp_i2c_config_t c = cfg400k();
    TEST_ASSERT_EQUAL(RP_I2C_STATUS_OK, rp_i2c_init(RP_I2C0, &c));
    TEST_ASSERT_EQUAL_HEX32(149U, I2C0FSHCNT);
    TEST_ASSERT_EQUAL_HEX32(147U, I2C0FSLCNT);
    TEST_ASSERT_EQUAL_HEX32(0x50U, I2C0TAR);
    TEST_ASSERT_EQUAL_HEX32(1U | (2UL << 1) | (1UL << 5), I2C0CON);
    TEST_ASSERT_EQUAL_HEX32(1U, I2C0ENABLE);
}

static void test_write_gated_by_tfnf(void)
{
    rp_i2c_config_t c = cfg400k();
    rp_i2c_init(RP_I2C0, &c);
    I2C0STATUS = 0U;
    TEST_ASSERT_EQUAL(RP_I2C_STATUS_NACK, rp_i2c_write_byte(RP_I2C0, 0x11U));
    I2C0STATUS = 1UL << 1;
    TEST_ASSERT_EQUAL(RP_I2C_STATUS_OK, rp_i2c_write_byte(RP_I2C0, 0x11U));
    TEST_ASSERT_EQUAL_HEX32(0x11U, I2C0DATACMD);
}

static void test_addr_7bit_limit_enforced(void)
{
    rp_i2c_config_t c = cfg400k();
    c.slave_addr = 0x80U;
    TEST_ASSERT_EQUAL(RP_I2C_STATUS_INVALID_PARAM, rp_i2c_init(RP_I2C1, &c));
    TEST_ASSERT_EQUAL_HEX32(0U, I2C1ENABLE);
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_counts_absorb_internal_latency);
    RUN_TEST(test_init_programs_counts_tar_con_enable_last);
    RUN_TEST(test_write_gated_by_tfnf);
    RUN_TEST(test_addr_7bit_limit_enforced);
    return UNITY_END();
}
#endif
