/* test_dig_out.c — active-level inversion + safe power-on state, bit-exact. */
#include "unity.h"
#include "xc.h"
#include "../../devices/dig_out/dig_out.h"

void setUp(void) { mock_sfr_reset(); }
void tearDown(void) {}

static dig_out_config_t relay_cfg(void)
{
    /* kit #6 relay: active-LOW trigger — the dangerous one */
    dig_out_config_t c = { 18U, DIG_OUT_ACTIVE_LOW };
    return c;
}

static void test_init_drives_inactive_immediately(void)
{
    dig_out_t dev;
    dig_out_config_t c = relay_cfg();
    TEST_ASSERT_EQUAL(DIG_OUT_STATUS_OK, dig_out_init(&dev, &c));
    /* active-LOW inactive = pin HIGH — coil OFF at power-on */
    TEST_ASSERT_EQUAL_HEX32(1UL << 18, SIO_GPIO_OUT);
    TEST_ASSERT_FALSE(dig_out_is_on(&dev));
}

static void test_relay_on_drives_pin_low(void)
{
    dig_out_t dev;
    dig_out_config_t c = relay_cfg();
    dig_out_init(&dev, &c);
    dig_out_set(&dev, true);
    TEST_ASSERT_EQUAL_HEX32(0U, SIO_GPIO_OUT & (1UL << 18));
    TEST_ASSERT_TRUE(dig_out_is_on(&dev));
}

static void test_active_high_buzzer_direct(void)
{
    dig_out_t dev;
    /* kit #16 active buzzer: HIGH = on */
    dig_out_config_t c = { 19U, DIG_OUT_ACTIVE_HIGH };
    dig_out_init(&dev, &c);
    TEST_ASSERT_EQUAL_HEX32(0U, SIO_GPIO_OUT & (1UL << 19));  /* off at boot */
    dig_out_set(&dev, true);
    TEST_ASSERT_EQUAL_HEX32(1UL << 19, SIO_GPIO_OUT);
}

static void test_toggle_roundtrip(void)
{
    dig_out_t dev;
    dig_out_config_t c = relay_cfg();
    dig_out_init(&dev, &c);
    dig_out_toggle(&dev);
    TEST_ASSERT_TRUE(dig_out_is_on(&dev));
    dig_out_toggle(&dev);
    TEST_ASSERT_FALSE(dig_out_is_on(&dev));
    TEST_ASSERT_EQUAL_HEX32(1UL << 18, SIO_GPIO_OUT);
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_drives_inactive_immediately);
    RUN_TEST(test_relay_on_drives_pin_low);
    RUN_TEST(test_active_high_buzzer_direct);
    RUN_TEST(test_toggle_roundtrip);
    return UNITY_END();
}
#endif
