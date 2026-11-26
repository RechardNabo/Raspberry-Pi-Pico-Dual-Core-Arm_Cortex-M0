/* test_rp_spi.c — cascaded divisor + orthogonal mode bits, bit-exact. */
#include "unity.h"
#include "xc.h"
#include "../../peripherals/spi/rp_spi.h"

void setUp(void) { mock_sfr_reset(); }
void tearDown(void) {}

static rp_spi_config_t cfg10m(void)
{
    rp_spi_config_t c;
    c.clk_hz = 125000000UL;
    c.clock_hz = 10000000UL;
    c.mode = RP_SPI_MODE_0;
    return c;
}

static void test_cascaded_divisor_never_overclocks(void)
{
    uint32_t cps, scr;
    rp_spi_divisors(125000000UL, 10000000UL, &cps, &scr);
    TEST_ASSERT_EQUAL_UINT32(2U, cps);
    TEST_ASSERT_EQUAL_UINT32(6U, scr);
    /* actual: 125e6/(2*7) = 8.93MHz <= 10MHz — never over */
}

static void test_init_programs_cr0_cpsr_enable_last(void)
{
    rp_spi_config_t c = cfg10m();
    TEST_ASSERT_EQUAL(RP_SPI_STATUS_OK, rp_spi_init(RP_SPI0, &c));
    TEST_ASSERT_EQUAL_HEX32(2U, SPI0CPSR);
    TEST_ASSERT_EQUAL_HEX32(7U | (6UL << 8), SPI0CR0);
    TEST_ASSERT_EQUAL_HEX32(1UL << 1, SPI0CR1);
}

static void test_mode3_sets_spo_and_sph(void)
{
    rp_spi_config_t c = cfg10m();
    c.mode = RP_SPI_MODE_3;
    rp_spi_init(RP_SPI1, &c);
    TEST_ASSERT_EQUAL_HEX32(7U | (6UL << 8) | (1UL << 6) | (1UL << 7), SPI1CR0);
}

static void test_exchange_through_fifo_flags(void)
{
    rp_spi_config_t c = cfg10m();
    uint8_t rx = 0U;
    rp_spi_init(RP_SPI0, &c);
    SPI0SR = (1UL << 1) | (1UL << 2);   /* TNF + RNE */
    SPI0DR = 0xA5U;                      /* looped-back byte waiting */
    TEST_ASSERT_EQUAL(RP_SPI_STATUS_OK, rp_spi_exchange_byte(RP_SPI0, 0xA5U, &rx));
    TEST_ASSERT_EQUAL_HEX8(0xA5U, rx);
}

static void test_timeout_when_fifo_never_ready(void)
{
    rp_spi_config_t c = cfg10m();
    rp_spi_init(RP_SPI0, &c);
    SPI0SR = 0U;
    TEST_ASSERT_EQUAL(RP_SPI_STATUS_INVALID_CLOCK, rp_spi_exchange_byte(RP_SPI0, 0U, 0));
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_cascaded_divisor_never_overclocks);
    RUN_TEST(test_init_programs_cr0_cpsr_enable_last);
    RUN_TEST(test_mode3_sets_spo_and_sph);
    RUN_TEST(test_exchange_through_fifo_flags);
    RUN_TEST(test_timeout_when_fifo_never_ready);
    return UNITY_END();
}
#endif
