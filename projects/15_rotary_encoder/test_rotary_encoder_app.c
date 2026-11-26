/* test_rotary_encoder_app.c — 3 tests: bounded position contract.
 * Direction-specific counting is pinned in the quad_enc device tests;
 * here we pin the APPLICATION contract: init, idle-stability, reset. */
#include "unity.h"
#include "xc.h"
#include "rotary_encoder_app.h"

static rotary_encoder_app_t app;

void setUp(void)
{
    mock_sfr_reset();
    rotary_encoder_app_init(&app, 6U, 7U, 8U);
    SIO_GPIO_IN = 1UL << 8;   /* button released (active-low) */
}
void tearDown(void) {}

static void test_position_zero_at_init(void)
{
    TEST_ASSERT_EQUAL_INT32(0, rotary_encoder_app_position(&app));
}

static void test_idle_input_never_moves_position(void)
{
    int i;
    for (i = 0; i < 50; i++) rotary_encoder_app_step(&app);
    TEST_ASSERT_EQUAL_INT32(0, rotary_encoder_app_position(&app));
}

static void test_button_reset_returns_position_to_zero(void)
{
    int i;
    app.position = 42;
    SIO_GPIO_IN &= ~(1UL << 8);   /* pressed: pin reads low */
    for (i = 0; i < 10; i++) rotary_encoder_app_step(&app);
    TEST_ASSERT_EQUAL_INT32(0, rotary_encoder_app_position(&app));
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_position_zero_at_init);
    RUN_TEST(test_idle_input_never_moves_position);
    RUN_TEST(test_button_reset_returns_position_to_zero);
    return UNITY_END();
}
#endif
