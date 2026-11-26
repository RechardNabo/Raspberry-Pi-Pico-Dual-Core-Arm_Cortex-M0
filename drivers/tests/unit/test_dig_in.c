/* test_dig_in.c — debounce integration + active-level, bit-exact on mock SFR. */
#include "unity.h"
#include "xc.h"
#include "../../devices/dig_in/dig_in.h"

void setUp(void) { mock_sfr_reset(); }
void tearDown(void) {}

static dig_in_config_t button_cfg(void)
{
    /* kit #28 button: active-LOW, pull-up — bounce-prone, debounce 5 */
    dig_in_config_t c = { 16U, DIG_IN_ACTIVE_LOW, 5U };
    return c;
}

static void test_init_configures_input_pin(void)
{
    dig_in_t dev;
    dig_in_config_t c = button_cfg();
    TEST_ASSERT_EQUAL(DIG_IN_STATUS_OK, dig_in_init(&dev, &c));
    TEST_ASSERT_EQUAL_HEX32(0U, SIO_GPIO_OE & (1UL << 16));   /* input: OE clear */
    TEST_ASSERT_FALSE(dig_in_state(&dev));
}

static void test_single_noise_spike_does_not_flip_state(void)
{
    dig_in_t dev;
    dig_in_config_t c = button_cfg();
    dig_in_init(&dev, &c);
    SIO_GPIO_IN = 0U;      /* pressed (active low) — one sample only */
    dig_in_sample(&dev);
    TEST_ASSERT_FALSE(dig_in_state(&dev));   /* 1 < 5: not yet */
    SIO_GPIO_IN = 1UL << 16;                 /* noise gone */
    dig_in_sample(&dev);
    dig_in_sample(&dev);
    TEST_ASSERT_FALSE(dig_in_state(&dev));
}

static void test_sustained_press_flips_after_debounce(void)
{
    dig_in_t dev;
    dig_in_config_t c = button_cfg();
    dig_in_init(&dev, &c);
    SIO_GPIO_IN = 0U;
    int i;
    for (i = 0; i < 5; i++) dig_in_sample(&dev);
    TEST_ASSERT_TRUE(dig_in_state(&dev));
}

static void test_active_high_no_debounce_immediate(void)
{
    dig_in_t dev;
    /* kit #10 touch: active-HIGH, digital output already clean */
    dig_in_config_t c = { 17U, DIG_IN_ACTIVE_HIGH, 0U };
    dig_in_init(&dev, &c);
    SIO_GPIO_IN = 1UL << 17;
    dig_in_sample(&dev);
    TEST_ASSERT_TRUE(dig_in_state(&dev));
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_configures_input_pin);
    RUN_TEST(test_single_noise_spike_does_not_flip_state);
    RUN_TEST(test_sustained_press_flips_after_debounce);
    RUN_TEST(test_active_high_no_debounce_immediate);
    return UNITY_END();
}
#endif
