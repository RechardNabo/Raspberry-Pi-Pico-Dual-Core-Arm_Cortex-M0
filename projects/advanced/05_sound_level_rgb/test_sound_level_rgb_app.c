/* test_sound_level_rgb_app.c — 3 tests pinning the meter contract. */
#include "unity.h"
#include "xc.h"
#include "sound_level_rgb_app.h"
#include "../../../drivers/devices/kit/kit.h"

static sound_level_rgb_app_t app;

void setUp(void)
{
    mock_sfr_reset();
    rp_adc_init();
    sound_level_rgb_app_init(&app, 0U, 3U, 4U, 5U);
    ADC_RESULT = 0U;
    sound_level_rgb_app_step(&app);
}
void tearDown(void) {}

static void test_silence_is_quiet_band(void)
{
    ADC_RESULT = 100U;
    int i;
    for (i = 0; i < 20; i++) sound_level_rgb_app_step(&app);
    TEST_ASSERT_EQUAL(SL_QUIET, sound_level_rgb_band(&app));
}

static void test_loud_sound_reaches_loud_band(void)
{
    ADC_RESULT = 3000U;
    sound_level_rgb_app_step(&app);
    TEST_ASSERT_EQUAL(SL_LOUD, sound_level_rgb_band(&app));
}

static void test_peak_decays_gradually_not_instantly(void)
{
    ADC_RESULT = 3000U;
    sound_level_rgb_app_step(&app);
    uint32_t peak_at_spike = sound_level_rgb_peak(&app);
    TEST_ASSERT_EQUAL_UINT32(3000U, peak_at_spike);

    /* sound stops: one tick later the peak must have decayed by exactly
     * SL_DECAY_PER_TICK — not dropped to the silent sample value */
    ADC_RESULT = 100U;
    sound_level_rgb_app_step(&app);
    TEST_ASSERT_EQUAL_UINT32(3000U - SL_DECAY_PER_TICK, sound_level_rgb_peak(&app));
    TEST_ASSERT_EQUAL(SL_LOUD, sound_level_rgb_band(&app));  /* still loud just after */

    /* after enough ticks it decays through moderate to quiet */
    int i;
    for (i = 0; i < 400; i++) sound_level_rgb_app_step(&app);
    TEST_ASSERT_EQUAL(SL_QUIET, sound_level_rgb_band(&app));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_silence_is_quiet_band);
    RUN_TEST(test_loud_sound_reaches_loud_band);
    RUN_TEST(test_peak_decays_gradually_not_instantly);
    return UNITY_END();
}
