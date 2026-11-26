/* test_pwm_out.c — RGB channel mixing + buzzer retune, bit-exact on mock PWM. */
#include "unity.h"
#include "xc.h"
#include "../../devices/pwm_out/pwm_out.h"

void setUp(void) { mock_sfr_reset(); }
void tearDown(void) {}

static void test_rgb_init_silences_all_channels(void)
{
    rgb_led_t dev;
    TEST_ASSERT_EQUAL(PWM_OUT_STATUS_OK,
        rgb_led_init(&dev, 0U, 0U, 1U, 0U, 2U, 0U, 125000000UL, 1000UL));
    TEST_ASSERT_EQUAL_HEX32(0U, PWM_CC[0]);
    TEST_ASSERT_EQUAL_HEX32(0U, PWM_CC[1]);
    TEST_ASSERT_EQUAL_HEX32(0U, PWM_CC[2]);
}

static void test_rgb_full_red_max_duty(void)
{
    rgb_led_t dev;
    rgb_led_init(&dev, 0U, 0U, 1U, 0U, 2U, 0U, 125000000UL, 1000UL);
    rgb_led_set(&dev, 255U, 0U, 0U);
    /* 255 -> 1000 permille -> level = top+1 = 62500 (1kHz, div2: top 62499) */
    TEST_ASSERT_EQUAL_HEX32(62500U, PWM_CC[0] & 0xFFFFU);
    TEST_ASSERT_EQUAL_HEX32(0U, PWM_CC[1]);
    TEST_ASSERT_EQUAL_HEX32(0U, PWM_CC[2]);
}

static void test_rgb_half_green_scales(void)
{
    rgb_led_t dev;
    rgb_led_init(&dev, 0U, 0U, 1U, 0U, 2U, 0U, 125000000UL, 1000UL);
    rgb_led_set(&dev, 0U, 128U, 0U);
    /* 128*1000/255 = 501 permille -> level 62500*501/1000 = 31312 */
    TEST_ASSERT_EQUAL_HEX32(31312U, PWM_CC[1] & 0xFFFFU);
}

static void test_buzzer_tone_retunes_slice(void)
{
    buzzer_t dev;
    TEST_ASSERT_EQUAL(PWM_OUT_STATUS_OK, buzzer_init(&dev, 3U, 0U, 125000000UL));
    TEST_ASSERT_EQUAL_HEX32(0U, PWM_CC[3]);          /* silent after init */
    TEST_ASSERT_EQUAL(PWM_OUT_STATUS_OK, buzzer_tone(&dev, 2000UL));
    /* 125e6/2000 = 62500 ticks -> div1 top 62499, 50% duty -> 31250 */
    TEST_ASSERT_EQUAL_HEX32(62499U, PWM_TOP[3]);
    TEST_ASSERT_EQUAL_HEX32(31250U, PWM_CC[3] & 0xFFFFU);
    TEST_ASSERT_TRUE(dev.sounding);
}

static void test_buzzer_off_silences_without_retune(void)
{
    buzzer_t dev;
    buzzer_init(&dev, 3U, 0U, 125000000UL);
    buzzer_tone(&dev, 2000UL);
    buzzer_off(&dev);
    TEST_ASSERT_EQUAL_HEX32(0U, PWM_CC[3]);
    TEST_ASSERT_EQUAL_HEX32(62499U, PWM_TOP[3]);     /* TOP left as-is */
    TEST_ASSERT_FALSE(dev.sounding);
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_rgb_init_silences_all_channels);
    RUN_TEST(test_rgb_full_red_max_duty);
    RUN_TEST(test_rgb_half_green_scales);
    RUN_TEST(test_buzzer_tone_retunes_slice);
    RUN_TEST(test_buzzer_off_silences_without_retune);
    return UNITY_END();
}
#endif
