/* test_knock_lock_app.c — 3 tests pinning the pattern contract. */
#include "unity.h"
#include "xc.h"
#include "knock_lock_app.h"
#include "../../../drivers/devices/kit/kit.h"

static knock_lock_app_t app;
static uint8_t knock_pin;

static void drive_knock(bool high)
{
    uint32_t bit = 1UL << knock_pin;
    if (high) SIO_GPIO_IN |= bit; else SIO_GPIO_IN &= ~bit;
}

void setUp(void)
{
    mock_sfr_reset();
    knock_pin = kit_default_pin(KIT_KNOCK);
    knock_lock_app_init(&app, knock_pin,
                        kit_default_pin(KIT_RELAY),
                        kit_default_pin(KIT_DUAL_LED));
    drive_knock(false);
}
void tearDown(void) {}

static void knock_once(void)
{
    drive_knock(true);
    knock_lock_app_step(&app);
    drive_knock(false);
    knock_lock_app_step(&app);
}

static void test_correct_pattern_unlocks(void)
{
    knock_once();
    knock_once();
    TEST_ASSERT_FALSE(knock_lock_unlocked(&app));
    knock_once();
    TEST_ASSERT_TRUE(knock_lock_unlocked(&app));
}

static void test_partial_pattern_decays_when_window_expires(void)
{
    knock_once();
    knock_once();
    TEST_ASSERT_FALSE(knock_lock_unlocked(&app));

    /* let the window expire without the third knock */
    uint32_t i;
    for (i = 0; i < KL_WINDOW_TICKS + 2U; i++) knock_lock_app_step(&app);

    /* now even two more quick knocks must not unlock: the count restarted */
    knock_once();
    TEST_ASSERT_FALSE(knock_lock_unlocked(&app));
}

static void test_unlock_relocks_automatically(void)
{
    knock_once();
    knock_once();
    knock_once();
    TEST_ASSERT_TRUE(knock_lock_unlocked(&app));

    uint32_t i;
    for (i = 0; i < KL_UNLOCK_TICKS + 2U; i++) knock_lock_app_step(&app);
    TEST_ASSERT_FALSE(knock_lock_unlocked(&app));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_correct_pattern_unlocks);
    RUN_TEST(test_partial_pattern_decays_when_window_expires);
    RUN_TEST(test_unlock_relocks_automatically);
    return UNITY_END();
}
