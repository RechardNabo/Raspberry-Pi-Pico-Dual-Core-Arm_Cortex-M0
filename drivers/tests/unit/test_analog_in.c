/* test_analog_in.c — integer scaling + channel validation, mock ADC. */
#include "unity.h"
#include "xc.h"
#include "../../devices/analog_in/analog_in.h"

void setUp(void) { mock_sfr_reset(); rp_adc_init(); }
void tearDown(void) {}

static analog_in_config_t lm35_cfg(void)
{
    /* LM35 (#21/#32): 10mV/C. centiC = raw x 330000 / 40950 */
    analog_in_config_t c = { 0U, 330000, 40950, 0 };
    return c;
}

static void test_lm35_scaling_math(void)
{
    analog_in_config_t c = lm35_cfg();
    /* raw 4095 -> 3.3V -> 3300mV -> 330C -> 33000 centiC */
    TEST_ASSERT_EQUAL_INT32(33000, analog_in_scale(4095U, &c));
    /* raw 124 -> 124x330000/40950 = 999.3 -> 999 centiC = 9.99C */
    TEST_ASSERT_EQUAL_INT32(999, analog_in_scale(124U, &c));
}

static void test_read_scaled_end_to_end(void)
{
    analog_in_t dev;
    analog_in_config_t c = lm35_cfg();
    int32_t centiC = 0;
    TEST_ASSERT_EQUAL(ANALOG_IN_STATUS_OK, analog_in_init(&dev, &c));
    ADC_RESULT = 4095U;
    TEST_ASSERT_EQUAL(ANALOG_IN_STATUS_OK, analog_in_read_scaled(&dev, &centiC));
    TEST_ASSERT_EQUAL_INT32(33000, centiC);
}

static void test_zero_denominator_rejected(void)
{
    analog_in_t dev;
    analog_in_config_t c = lm35_cfg();
    c.scale_den = 0;
    TEST_ASSERT_EQUAL(ANALOG_IN_STATUS_INVALID_CONFIG, analog_in_init(&dev, &c));
}

static void test_invalid_channel_rejected(void)
{
    analog_in_t dev;
    analog_in_config_t c = lm35_cfg();
    c.adc_channel = 9U;
    TEST_ASSERT_EQUAL(ANALOG_IN_STATUS_INVALID_CONFIG, analog_in_init(&dev, &c));
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_lm35_scaling_math);
    RUN_TEST(test_read_scaled_end_to_end);
    RUN_TEST(test_zero_denominator_rejected);
    RUN_TEST(test_invalid_channel_rejected);
    return UNITY_END();
}
#endif
