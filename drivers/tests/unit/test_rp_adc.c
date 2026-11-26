/* test_rp_adc.c — input select encoding + conversion model, bit-exact. */
#include "unity.h"
#include "xc.h"
#include "../../peripherals/adc/rp_adc.h"

void setUp(void) { mock_sfr_reset(); }
void tearDown(void) {}

static void test_init_enables_adc(void)
{
    TEST_ASSERT_EQUAL(RP_ADC_STATUS_OK, rp_adc_init());
    TEST_ASSERT_EQUAL_HEX32(1U, ADC_CS);
}

static void test_read_selects_channel_in_ainsel_bits(void)
{
    uint16_t raw = 0U;
    rp_adc_init();
    ADC_RESULT = 2048U;
    TEST_ASSERT_EQUAL(RP_ADC_STATUS_OK, rp_adc_read(2U, &raw));
    TEST_ASSERT_EQUAL_HEX32(1U | (2UL << 12) | (1UL << 8), ADC_CS);
    TEST_ASSERT_EQUAL_UINT16(2048U, raw);
}

static void test_invalid_channel_rejected(void)
{
    uint16_t raw = 0U;
    TEST_ASSERT_EQUAL(RP_ADC_STATUS_INVALID_CHANNEL, rp_adc_read(5U, &raw));
}

static void test_millivolt_conversion(void)
{
    TEST_ASSERT_EQUAL_UINT16(1650U, rp_adc_to_mv(2048U));
    TEST_ASSERT_EQUAL_UINT16(3300U, rp_adc_to_mv(4095U));
    TEST_ASSERT_EQUAL_UINT16(0U, rp_adc_to_mv(0U));
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_enables_adc);
    RUN_TEST(test_read_selects_channel_in_ainsel_bits);
    RUN_TEST(test_invalid_channel_rejected);
    RUN_TEST(test_millivolt_conversion);
    return UNITY_END();
}
#endif
