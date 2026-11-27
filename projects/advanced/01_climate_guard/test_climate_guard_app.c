/* test_climate_guard_app.c — 3 tests pinning the dual-path fusion contract. */
#include "unity.h"
#include "xc.h"
#include "climate_guard_app.h"
#include "../../../drivers/devices/kit/kit.h"

static climate_guard_app_t app;

void setUp(void)
{
    mock_sfr_reset();
    rp_adc_init();
    climate_guard_app_init(&app, 0U,
                           kit_default_pin(KIT_RELAY),
                           kit_default_pin(KIT_BUZZER_ACTIVE));
    ADC_RESULT = 2000U;   /* inside the deadband */
    climate_guard_app_step(&app);
}
void tearDown(void) {}

static void test_power_on_deadband_means_relay_off_buzzer_silent(void)
{
    TEST_ASSERT_FALSE(climate_guard_heating(&app));
    TEST_ASSERT_FALSE(climate_guard_alarm(&app));
}

static void test_heating_engages_below_low_holds_deadband_releases_above_high(void)
{
    ADC_RESULT = 1000U;   /* cold: engage heating */
    climate_guard_app_step(&app);
    TEST_ASSERT_TRUE(climate_guard_heating(&app));

    ADC_RESULT = 2000U;   /* deadband: must hold heating */
    int i;
    for (i = 0; i < 5; i++) climate_guard_app_step(&app);
    TEST_ASSERT_TRUE(climate_guard_heating(&app));

    ADC_RESULT = 3000U;   /* warm: release */
    climate_guard_app_step(&app);
    TEST_ASSERT_FALSE(climate_guard_heating(&app));
}

static void test_alarm_independent_of_relay_state(void)
{
    /* heat the space: relay active */
    ADC_RESULT = 1000U;
    climate_guard_app_step(&app);
    TEST_ASSERT_TRUE(climate_guard_heating(&app));
    TEST_ASSERT_FALSE(climate_guard_alarm(&app));

    /* runaway temperature: buzzer must fire even while relay state is driven */
    ADC_RESULT = 4000U;
    climate_guard_app_step(&app);
    TEST_ASSERT_TRUE(climate_guard_alarm(&app));
    TEST_ASSERT_FALSE(climate_guard_heating(&app));  /* relay released — too hot */

    /* cooling back into deadband silences buzzer */
    ADC_RESULT = 2000U;
    climate_guard_app_step(&app);
    TEST_ASSERT_FALSE(climate_guard_alarm(&app));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_power_on_deadband_means_relay_off_buzzer_silent);
    RUN_TEST(test_heating_engages_below_low_holds_deadband_releases_above_high);
    RUN_TEST(test_alarm_independent_of_relay_state);
    return UNITY_END();
}
