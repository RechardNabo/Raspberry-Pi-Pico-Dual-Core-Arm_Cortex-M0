/* test_auto_light_app.c — 3 tests pinning override semantics. */
#include "unity.h"
#include "xc.h"
#include "auto_light_app.h"
#include "../../../drivers/devices/kit/kit.h"

static auto_light_app_t app;
static uint8_t btn_pin;

static void drive_btn(bool high)
{
    uint32_t bit = 1UL << btn_pin;
    if (high) SIO_GPIO_IN |= bit; else SIO_GPIO_IN &= ~bit;
}

void setUp(void)
{
    mock_sfr_reset();
    rp_adc_init();
    btn_pin = kit_default_pin(KIT_BUTTON);
    auto_light_app_init(&app, 0U, btn_pin, kit_default_pin(KIT_RELAY));
    drive_btn(true);   /* inactive = HIGH */
}
void tearDown(void) {}

/* button active-LOW, 5-sample debounce: pressed = LOW */
static void press(void)
{
    int i;
    drive_btn(false);
    for (i = 0; i < 8; i++) auto_light_app_step(&app);
    drive_btn(true);
    for (i = 0; i < 8; i++) auto_light_app_step(&app);
}

static void test_night_on_day_off_deadband_holds(void)
{
    ADC_RESULT = 500U;    /* dark */
    auto_light_app_step(&app);
    TEST_ASSERT_TRUE(auto_light_output(&app));

    ADC_RESULT = 1600U;   /* deadband: hold night */
    int i;
    for (i = 0; i < 5; i++) auto_light_app_step(&app);
    TEST_ASSERT_TRUE(auto_light_output(&app));

    ADC_RESULT = 3000U;   /* bright day */
    auto_light_app_step(&app);
    TEST_ASSERT_FALSE(auto_light_output(&app));
}

static void test_override_inverts_output(void)
{
    ADC_RESULT = 500U;    /* dark — lights on */
    auto_light_app_step(&app);
    TEST_ASSERT_TRUE(auto_light_output(&app));

    press();              /* override: force off */
    auto_light_app_step(&app);
    TEST_ASSERT_TRUE(auto_light_overridden(&app));
    TEST_ASSERT_FALSE(auto_light_output(&app));

    press();              /* override again: back to automatic */
    auto_light_app_step(&app);
    TEST_ASSERT_FALSE(auto_light_overridden(&app));
    TEST_ASSERT_TRUE(auto_light_output(&app));
}

static void test_override_cleared_at_next_transition(void)
{
    ADC_RESULT = 500U;    /* night */
    auto_light_app_step(&app);
    press();              /* override on */
    auto_light_app_step(&app);
    TEST_ASSERT_TRUE(auto_light_overridden(&app));

    /* day arrives: transition clears the override */
    ADC_RESULT = 3000U;
    auto_light_app_step(&app);
    TEST_ASSERT_FALSE(auto_light_overridden(&app));
    TEST_ASSERT_FALSE(auto_light_output(&app));   /* normal day behavior */
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_night_on_day_off_deadband_holds);
    RUN_TEST(test_override_inverts_output);
    RUN_TEST(test_override_cleared_at_next_transition);
    return UNITY_END();
}
