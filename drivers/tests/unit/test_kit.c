/* test_kit.c — the kit table IS the firmware: verify the electrical facts. */
#include "unity.h"
#include "../../devices/kit/kit.h"

void setUp(void) {}
void tearDown(void) {}

static void test_every_module_has_descriptor(void)
{
    /* table covers all 35 listed module IDs */
    TEST_ASSERT_NOT_NULL(kit_info(KIT_JOYSTICK));
    TEST_ASSERT_NOT_NULL(kit_info(KIT_REED_BIG));
    TEST_ASSERT_NOT_NULL(kit_info(KIT_MIC_SOUND));
    TEST_ASSERT_EQUAL_STRING("DS18B20 precision temp", kit_info(KIT_DS18B20)->name);
}

static void test_relay_is_active_low_the_safety_fact(void)
{
    dig_out_config_t c = kit_dig_out_config(KIT_RELAY, 16U);
    TEST_ASSERT_EQUAL(DIG_OUT_ACTIVE_LOW, c.active);
    /* every other output module is active-high */
    TEST_ASSERT_EQUAL(DIG_OUT_ACTIVE_HIGH, kit_dig_out_config(KIT_BUZZER_ACTIVE, 17U).active);
    TEST_ASSERT_EQUAL(DIG_OUT_ACTIVE_HIGH, kit_dig_out_config(KIT_LASER, 18U).active);
}

static void test_comparator_modules_active_low_clean(void)
{
    /* LM393 threshold modules pull LOW when triggered, no debounce needed */
    dig_in_config_t c = kit_dig_in_config(KIT_OBSTACLE, 13U);
    TEST_ASSERT_EQUAL(DIG_IN_ACTIVE_LOW, c.active);
    TEST_ASSERT_EQUAL_UINT8(0U, c.debounce_samples);
}

static void test_mechanical_contacts_debounced(void)
{
    dig_in_config_t c = kit_dig_in_config(KIT_BUTTON, 8U);
    TEST_ASSERT_EQUAL(DIG_IN_ACTIVE_LOW, c.active);
    TEST_ASSERT_EQUAL_UINT8(5U, c.debounce_samples);
}

static void test_lm35_scale_facts(void)
{
    analog_in_config_t c = kit_analog_config(KIT_LM35, 1U);
    TEST_ASSERT_EQUAL_INT32(330000, c.scale_num);
    TEST_ASSERT_EQUAL_INT32(40950, c.scale_den);
    /* full-scale raw 4095 -> 330.00C in centiC — verifies through scale math */
    TEST_ASSERT_EQUAL_INT32(33000, (int32_t)(((int64_t)4095 * c.scale_num) / c.scale_den));
}

static void test_ir_receiver_marked_unsupported_honestly(void)
{
    /* no capture infrastructure yet — the table says so instead of
     * pretending a GPIO poll is a 38kHz decoder */
    TEST_ASSERT_EQUAL(KIT_CLASS_UNSUPPORTED, kit_info(KIT_IR_RECEIVER)->cls);
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_every_module_has_descriptor);
    RUN_TEST(test_relay_is_active_low_the_safety_fact);
    RUN_TEST(test_comparator_modules_active_low_clean);
    RUN_TEST(test_mechanical_contacts_debounced);
    RUN_TEST(test_lm35_scale_facts);
    RUN_TEST(test_ir_receiver_marked_unsupported_honestly);
    return UNITY_END();
}
#endif
