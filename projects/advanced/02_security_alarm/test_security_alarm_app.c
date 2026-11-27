/* test_security_alarm_app.c — 3 tests pinning the arming FSM contract. */
#include "unity.h"
#include "xc.h"
#include "security_alarm_app.h"
#include "../../../drivers/devices/kit/kit.h"

static security_alarm_app_t app;
static uint8_t vib_pin, btn_pin;

static void drive(uint8_t pin, bool high)
{
    uint32_t bit = 1UL << pin;
    if (high) SIO_GPIO_IN |= bit; else SIO_GPIO_IN &= ~bit;
}

void setUp(void)
{
    mock_sfr_reset();
    vib_pin = kit_default_pin(KIT_VIBRATION);
    btn_pin = kit_default_pin(KIT_BUTTON);
    security_alarm_app_init(&app, vib_pin, btn_pin,
                            kit_default_pin(KIT_BUZZER_ACTIVE),
                            kit_default_pin(KIT_DUAL_LED));
    drive(vib_pin, true);    /* HIGH = inactive for active-low sensor */
    drive(btn_pin, true);
}
void tearDown(void) {}

/* button is active-LOW with 5-sample debounce: pressed = pin LOW.
 * hold the level long enough for the integrator both ways. */
static void press_button_short(void)
{
    int i;
    drive(btn_pin, false);   /* LOW = pressed */
    for (i = 0; i < 8; i++) security_alarm_app_step(&app);
    drive(btn_pin, true);    /* HIGH = released */
    for (i = 0; i < 8; i++) security_alarm_app_step(&app);
}

static void test_vibration_means_nothing_when_disarmed(void)
{
    int i;
    for (i = 0; i < 3; i++) {
        drive(vib_pin, false);   /* LOW = vibration active */
        security_alarm_app_step(&app);
        security_alarm_app_step(&app);
        security_alarm_app_step(&app);
        drive(vib_pin, true);
        security_alarm_app_step(&app);
        security_alarm_app_step(&app);
        security_alarm_app_step(&app);
    }
    TEST_ASSERT_EQUAL(SA_DISARMED, security_alarm_state(&app));
    TEST_ASSERT_FALSE(security_alarm_sounding(&app));
}

static void test_alarm_only_after_armed_and_triggered(void)
{
    press_button_short();
    TEST_ASSERT_EQUAL(SA_ARMING, security_alarm_state(&app));

    /* vibration during exit delay: still silent */
    drive(vib_pin, false);
    security_alarm_app_step(&app);
    security_alarm_app_step(&app);
    security_alarm_app_step(&app);
    drive(vib_pin, true);
    security_alarm_app_step(&app);
    security_alarm_app_step(&app);
    security_alarm_app_step(&app);
    TEST_ASSERT_FALSE(security_alarm_sounding(&app));

    /* run out the exit delay */
    uint32_t i;
    for (i = 0; i < SA_EXIT_DELAY_TICKS + 2U; i++) security_alarm_app_step(&app);
    TEST_ASSERT_EQUAL(SA_ARMED, security_alarm_state(&app));

    /* vibration edge now triggers the alarm */
    drive(vib_pin, false);
    security_alarm_app_step(&app);
    security_alarm_app_step(&app);
    security_alarm_app_step(&app);
    TEST_ASSERT_EQUAL(SA_ALARM, security_alarm_state(&app));
    TEST_ASSERT_TRUE(security_alarm_sounding(&app));
}

static void test_alarm_latches_until_long_hold_disarm(void)
{
    press_button_short();
    uint32_t i;
    for (i = 0; i < SA_EXIT_DELAY_TICKS + 2U; i++) security_alarm_app_step(&app);
    drive(vib_pin, false);
    security_alarm_app_step(&app);
    security_alarm_app_step(&app);
    security_alarm_app_step(&app);
    TEST_ASSERT_TRUE(security_alarm_sounding(&app));

    /* trigger stops — alarm must persist */
    drive(vib_pin, true);
    for (i = 0; i < 20U; i++) security_alarm_app_step(&app);
    TEST_ASSERT_TRUE(security_alarm_sounding(&app));

    /* long hold disarms: LOW = pressed, held long */
    drive(btn_pin, false);
    for (i = 0; i < SA_DISARM_HOLD_TICKS + 12U; i++) security_alarm_app_step(&app);
    drive(btn_pin, true);
    security_alarm_app_step(&app);
    TEST_ASSERT_EQUAL(SA_DISARMED, security_alarm_state(&app));
    TEST_ASSERT_FALSE(security_alarm_sounding(&app));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_vibration_means_nothing_when_disarmed);
    RUN_TEST(test_alarm_only_after_armed_and_triggered);
    RUN_TEST(test_alarm_latches_until_long_hold_disarm);
    return UNITY_END();
}
