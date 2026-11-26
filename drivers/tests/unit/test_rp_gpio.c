/* test_rp_gpio.c — SIO atomic-alias semantics, bit-exact. */
#include "unity.h"
#include "xc.h"
#include "../../peripherals/gpio/rp_gpio.h"

void setUp(void) { mock_sfr_reset(); }
void tearDown(void) {}

static void test_output_enable_via_alias(void)
{
    TEST_ASSERT_EQUAL(RP_GPIO_STATUS_OK, rp_gpio_set_output(25U));
    TEST_ASSERT_EQUAL_HEX32(1UL << 25, SIO_GPIO_OE_SET);
    TEST_ASSERT_EQUAL_HEX32(1UL << 25, SIO_GPIO_OE);
}

static void test_write_set_clear_aliases(void)
{
    rp_gpio_set_output(25U);
    rp_gpio_write(25U, true);
    TEST_ASSERT_EQUAL_HEX32(1UL << 25, SIO_GPIO_OUT);
    rp_gpio_write(25U, false);
    TEST_ASSERT_EQUAL_HEX32(0U, SIO_GPIO_OUT);
}

static void test_toggle_uses_xor_alias(void)
{
    rp_gpio_write(2U, true);
    rp_gpio_toggle(2U);
    TEST_ASSERT_EQUAL_HEX32(0U, SIO_GPIO_OUT);
    rp_gpio_toggle(2U);
    TEST_ASSERT_EQUAL_HEX32(1UL << 2, SIO_GPIO_OUT);
}

static void test_invalid_pin_rejected(void)
{
    TEST_ASSERT_EQUAL(RP_GPIO_STATUS_INVALID_PIN, rp_gpio_set_output(30U));
    TEST_ASSERT_EQUAL_HEX32(0U, SIO_GPIO_OE_SET);
}

static void test_read_reflects_input(void)
{
    SIO_GPIO_IN = 1UL << 15;
    TEST_ASSERT_TRUE(rp_gpio_read(15U));
    TEST_ASSERT_FALSE(rp_gpio_read(14U));
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_output_enable_via_alias);
    RUN_TEST(test_write_set_clear_aliases);
    RUN_TEST(test_toggle_uses_xor_alias);
    RUN_TEST(test_invalid_pin_rejected);
    RUN_TEST(test_read_reflects_input);
    return UNITY_END();
}
#endif
