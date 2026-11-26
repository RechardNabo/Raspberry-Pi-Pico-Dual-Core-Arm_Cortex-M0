/* test_security_alarm.c — full alarm lifecycle end-to-end. */
#include "unity.h"
#include "../../apps/security_alarm/security_alarm.h"

void setUp(void) {}
void tearDown(void) {}

static void step_n(security_alarm_t *app, uint32_t n)
{
    uint32_t i;
    for (i = 0U; i < n; i++) security_alarm_step(app);
}

static void test_arm_sequence_exit_delay_then_armed(void)
{
    security_alarm_t app;
    security_alarm_init(&app);
    security_alarm_inputs(&app, true, false, true);   /* press arm */
    security_alarm_step(&app);
    TEST_ASSERT_EQUAL(ALARM_STATE_EXIT_DELAY, app.state);
    security_alarm_inputs(&app, true, false, false);  /* release */
    step_n(&app, ALARM_EXIT_DELAY_TICKS);
    TEST_ASSERT_EQUAL(ALARM_STATE_ARMED, app.state);
    TEST_ASSERT_TRUE(security_alarm_status_led_red(&app));
    TEST_ASSERT_FALSE(security_alarm_siren_on(&app));
}

static void test_door_open_entry_delay_then_alarm(void)
{
    security_alarm_t app;
    security_alarm_init(&app);
    security_alarm_inputs(&app, true, false, true);
    security_alarm_step(&app);
    security_alarm_inputs(&app, true, false, false);
    step_n(&app, ALARM_EXIT_DELAY_TICKS);
    /* door opens */
    security_alarm_inputs(&app, false, false, false);
    security_alarm_step(&app);
    TEST_ASSERT_EQUAL(ALARM_STATE_ENTRY_DELAY, app.state);
    TEST_ASSERT_FALSE(security_alarm_siren_on(&app));   /* grace: not yet */
    step_n(&app, ALARM_ENTRY_DELAY_TICKS);
    TEST_ASSERT_EQUAL(ALARM_STATE_ALARM, app.state);
    TEST_ASSERT_TRUE(security_alarm_siren_on(&app));
    TEST_ASSERT_EQUAL_UINT32(1U, app.alarm_count);
}

static void test_disarm_during_entry_delay_prevents_alarm(void)
{
    security_alarm_t app;
    security_alarm_init(&app);
    security_alarm_inputs(&app, true, false, true);
    security_alarm_step(&app);
    security_alarm_inputs(&app, true, false, false);
    step_n(&app, ALARM_EXIT_DELAY_TICKS);
    security_alarm_inputs(&app, false, false, false);
    security_alarm_step(&app);
    /* owner presses disarm within the grace window */
    security_alarm_inputs(&app, false, false, true);
    security_alarm_step(&app);
    TEST_ASSERT_EQUAL(ALARM_STATE_DISARMED, app.state);
    TEST_ASSERT_EQUAL_UINT32(0U, app.alarm_count);
}

static void test_shock_while_armed_is_instant_alarm(void)
{
    security_alarm_t app;
    security_alarm_init(&app);
    security_alarm_inputs(&app, true, false, true);
    security_alarm_step(&app);
    security_alarm_inputs(&app, true, false, false);
    step_n(&app, ALARM_EXIT_DELAY_TICKS);
    security_alarm_inputs(&app, true, true, false);   /* window break */
    security_alarm_step(&app);
    TEST_ASSERT_EQUAL(ALARM_STATE_ALARM, app.state);   /* no grace for glass */
}

static void test_alarm_latches_until_qualified_reset(void)
{
    security_alarm_t app;
    security_alarm_init(&app);
    security_alarm_inputs(&app, true, false, true);
    security_alarm_step(&app);
    security_alarm_inputs(&app, true, false, false);
    step_n(&app, ALARM_EXIT_DELAY_TICKS);
    security_alarm_inputs(&app, true, true, false);
    security_alarm_step(&app);
    /* intruder presses button with door open: must NOT reset */
    security_alarm_inputs(&app, false, false, true);
    security_alarm_step(&app);
    TEST_ASSERT_EQUAL(ALARM_STATE_ALARM, app.state);
    /* door closed + press: legitimate reset */
    security_alarm_inputs(&app, true, false, true);
    security_alarm_step(&app);
    TEST_ASSERT_EQUAL(ALARM_STATE_DISARMED, app.state);
}

static void test_tamper_probe_while_disarmed_counted_not_alarm(void)
{
    security_alarm_t app;
    security_alarm_init(&app);
    security_alarm_inputs(&app, true, true, false);
    security_alarm_step(&app);
    TEST_ASSERT_EQUAL(ALARM_STATE_DISARMED, app.state);
    TEST_ASSERT_EQUAL_UINT32(1U, app.tamper_count);
    TEST_ASSERT_EQUAL_UINT32(0U, app.alarm_count);
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_arm_sequence_exit_delay_then_armed);
    RUN_TEST(test_door_open_entry_delay_then_alarm);
    RUN_TEST(test_disarm_during_entry_delay_prevents_alarm);
    RUN_TEST(test_shock_while_armed_is_instant_alarm);
    RUN_TEST(test_alarm_latches_until_qualified_reset);
    RUN_TEST(test_tamper_probe_while_disarmed_counted_not_alarm);
    return UNITY_END();
}
#endif
