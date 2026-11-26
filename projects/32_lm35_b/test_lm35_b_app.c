/* test_lm35_b_app.c — 3 tests: threshold events with flap-proof hysteresis. */
#include "unity.h"
#include "xc.h"
#include "lm35_b_app.h"
#include "../../drivers/devices/kit/kit.h"

static lm35_b_app_t app;

void setUp(void)
{
    mock_sfr_reset();
    rp_adc_init();
    lm35_b_app_init(&app, 0U);
    ADC_RESULT = 1000U;
}
void tearDown(void) {}

static void test_crossing_high_threshold_fires_one_event(void)
{
    (void)lm35_b_app_step(&app);
    TEST_ASSERT_EQUAL_UINT32(0U, lm35_b_app_events(&app));
    ADC_RESULT = 3000U;
    TEST_ASSERT_TRUE(lm35_b_app_step(&app));
    TEST_ASSERT_EQUAL_UINT32(1U, lm35_b_app_events(&app));
    TEST_ASSERT_FALSE(lm35_b_app_step(&app));
    TEST_ASSERT_EQUAL_UINT32(1U, lm35_b_app_events(&app));
}

static void test_noise_inside_deadband_does_not_flap(void)
{
    ADC_RESULT = 3000U;
    (void)lm35_b_app_step(&app);
    TEST_ASSERT_EQUAL_UINT32(1U, lm35_b_app_events(&app));
    ADC_RESULT = 2000U;
    int i;
    for (i = 0; i < 10; i++) (void)lm35_b_app_step(&app);
    TEST_ASSERT_EQUAL_UINT32(1U, lm35_b_app_events(&app));
    ADC_RESULT = 3000U;
    for (i = 0; i < 5; i++) (void)lm35_b_app_step(&app);
    TEST_ASSERT_EQUAL_UINT32(1U, lm35_b_app_events(&app));
}

static void test_full_drop_below_low_rearms(void)
{
    ADC_RESULT = 3000U;
    (void)lm35_b_app_step(&app);
    ADC_RESULT = 500U;
    (void)lm35_b_app_step(&app);
    ADC_RESULT = 3000U;
    TEST_ASSERT_TRUE(lm35_b_app_step(&app));
    TEST_ASSERT_EQUAL_UINT32(2U, lm35_b_app_events(&app));
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_crossing_high_threshold_fires_one_event);
    RUN_TEST(test_noise_inside_deadband_does_not_flap);
    RUN_TEST(test_full_drop_below_low_rearms);
    return UNITY_END();
}
#endif
