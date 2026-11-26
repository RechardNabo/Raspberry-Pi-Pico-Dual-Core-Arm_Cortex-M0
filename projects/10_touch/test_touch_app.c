/* test_touch_app.c — 3 tests pinning the project's event-counting contract. */
#include "unity.h"
#include "xc.h"
#include "touch_app.h"
#include "../../drivers/devices/kit/kit.h"

static touch_app_t app;
static dig_in_config_t cfg;
static uint8_t pin;

void setUp(void)
{
    mock_sfr_reset();
    pin = kit_default_pin(KIT_TOUCH);
    cfg = kit_dig_in_config(KIT_TOUCH, pin);
    touch_app_init(&app, pin);
}
void tearDown(void) {}

static void drive_active(bool on)
{
    const uint32_t bit = 1UL << pin;
    const bool high = (cfg.active == DIG_IN_ACTIVE_HIGH) ? on : !on;
    if (high) SIO_GPIO_IN |= bit; else SIO_GPIO_IN &= ~bit;
}

static void settle(int ticks)
{
    int i;
    for (i = 0; i < ticks; i++) (void)touch_app_step(&app);
}

static void test_init_is_clean(void)
{
    TEST_ASSERT_EQUAL_UINT32(0U, touch_app_events(&app));
    TEST_ASSERT_EQUAL_HEX32(0U, SIO_GPIO_OE & (1UL << pin));   /* input, not output */
}

static void test_one_activation_counts_once_even_when_held(void)
{
    drive_active(true);
    settle(12);
    TEST_ASSERT_EQUAL_UINT32(1U, touch_app_events(&app));
    settle(20);   /* still held — must NOT recount */
    TEST_ASSERT_EQUAL_UINT32(1U, touch_app_events(&app));
}

static void test_release_and_reactivation_counts_again(void)
{
    drive_active(true);
    settle(12);
    drive_active(false);
    settle(12);
    drive_active(true);
    settle(12);
    TEST_ASSERT_EQUAL_UINT32(2U, touch_app_events(&app));
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_is_clean);
    RUN_TEST(test_one_activation_counts_once_even_when_held);
    RUN_TEST(test_release_and_reactivation_counts_again);
    return UNITY_END();
}
#endif
