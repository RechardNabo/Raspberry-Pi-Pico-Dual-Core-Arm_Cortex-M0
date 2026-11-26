/* test_rp_pwm.c — TOP frequency math + packed CC duty, bit-exact. */
#include "unity.h"
#include "xc.h"
#include "../../peripherals/pwm/rp_pwm.h"

void setUp(void) { mock_sfr_reset(); }
void tearDown(void) {}

static rp_pwm_config_t cfg1k(void)
{
    rp_pwm_config_t c;
    c.clk_hz = 125000000UL;
    c.frequency = 1000UL;
    c.channel = 0U;
    return c;
}

static void test_top_for_1khz(void)
{
    TEST_ASSERT_EQUAL_UINT32(124999U, rp_pwm_top_for_freq(125000000UL, 1000UL));
}

static void test_init_programs_top_div_enable_last(void)
{
    rp_pwm_config_t c = cfg1k();
    TEST_ASSERT_EQUAL(RP_PWM_STATUS_OK, rp_pwm_init(3U, &c));
    TEST_ASSERT_EQUAL_HEX32(62499U, PWM_TOP[3]);
    TEST_ASSERT_EQUAL_HEX32(2UL << 4, PWM_DIV[3]);
    TEST_ASSERT_EQUAL_HEX32(1U, PWM_CSR[3]);
}

static void test_duty_50_percent_channel_a(void)
{
    rp_pwm_config_t c = cfg1k();
    rp_pwm_init(3U, &c);
    TEST_ASSERT_EQUAL(RP_PWM_STATUS_OK, rp_pwm_set_duty_permille(3U, 0U, 500U));
    TEST_ASSERT_EQUAL_HEX32(31250U, PWM_CC[3] & 0xFFFFU);
}

static void test_duty_channel_b_in_high_halfword(void)
{
    rp_pwm_config_t c = cfg1k();
    rp_pwm_init(3U, &c);
    rp_pwm_set_duty_permille(3U, 1U, 250U);
    TEST_ASSERT_EQUAL_HEX32(15625UL << 16, PWM_CC[3] & 0xFFFF0000UL);
}

static void test_over_100_percent_rejected(void)
{
    rp_pwm_config_t c = cfg1k();
    rp_pwm_init(3U, &c);
    TEST_ASSERT_EQUAL(RP_PWM_STATUS_INVALID_PARAM, rp_pwm_set_duty_permille(3U, 0U, 1001U));
    TEST_ASSERT_EQUAL_HEX32(0U, PWM_CC[3]);
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_top_for_1khz);
    RUN_TEST(test_init_programs_top_div_enable_last);
    RUN_TEST(test_duty_50_percent_channel_a);
    RUN_TEST(test_duty_channel_b_in_high_halfword);
    RUN_TEST(test_over_100_percent_rejected);
    return UNITY_END();
}
#endif
