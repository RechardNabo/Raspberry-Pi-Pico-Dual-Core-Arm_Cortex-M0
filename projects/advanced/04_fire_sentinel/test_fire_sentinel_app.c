/* test_fire_sentinel_app.c — 3 tests pinning the correlation contract. */
#include "unity.h"
#include "xc.h"
#include "fire_sentinel_app.h"
#include "../../../drivers/devices/kit/kit.h"

static fire_sentinel_app_t app;
static uint8_t flame_pin;

static void drive_flame(bool high)
{
    uint32_t bit = 1UL << flame_pin;
    if (high) SIO_GPIO_IN |= bit; else SIO_GPIO_IN &= ~bit;
}

/* flame sensor is active-LOW with debounce: flame present = pin LOW,
 * held several samples so the integrator registers it */
static void flame_present(bool on)
{
    int i;
    drive_flame(!on);
    for (i = 0; i < 8; i++) fire_sentinel_app_step(&app);
}

void setUp(void)
{
    mock_sfr_reset();
    rp_adc_init();
    flame_pin = kit_default_pin(KIT_FLAME);
    fire_sentinel_app_init(&app, flame_pin, 0U,
                           kit_default_pin(KIT_BUZZER_ACTIVE),
                           kit_default_pin(KIT_DUAL_LED));
    drive_flame(true);   /* no flame = HIGH */
    ADC_RESULT = 1000U;   /* room temperature */
}
void tearDown(void) {}

static void test_flame_alone_at_low_temp_is_warning_not_alarm(void)
{
    flame_present(true);
    TEST_ASSERT_EQUAL(FS_WARNING, fire_sentinel_level(&app));
    /* WARNING means LED only — the buzzer output must stay silent:
     * verified through the level contract since buzzer = (level == ALARM) */
}

static void test_flame_plus_heat_is_full_alarm(void)
{
    flame_present(true);
    ADC_RESULT = 3500U;   /* hot */
    fire_sentinel_app_step(&app);
    TEST_ASSERT_EQUAL(FS_ALARM, fire_sentinel_level(&app));
}

static void test_heat_without_flame_is_warning_only(void)
{
    flame_present(false);
    ADC_RESULT = 3500U;
    fire_sentinel_app_step(&app);
    TEST_ASSERT_EQUAL(FS_WARNING, fire_sentinel_level(&app));

    /* clearing both returns to CLEAR */
    ADC_RESULT = 1000U;
    fire_sentinel_app_step(&app);
    TEST_ASSERT_EQUAL(FS_CLEAR, fire_sentinel_level(&app));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_flame_alone_at_low_temp_is_warning_not_alarm);
    RUN_TEST(test_flame_plus_heat_is_full_alarm);
    RUN_TEST(test_heat_without_flame_is_warning_only);
    return UNITY_END();
}
