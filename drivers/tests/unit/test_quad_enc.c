/* test_quad_enc.c — Gray-code state machine: valid steps count, noise rejected. */
#include "unity.h"
#include "xc.h"
#include "../../devices/quad_enc/quad_enc.h"

void setUp(void) { mock_sfr_reset(); }
void tearDown(void) {}

/* pins: A=20 (state bit1), B=21 (state bit0) */
static quad_enc_config_t ky040_cfg(void)
{
    quad_enc_config_t c = { 20U, 21U, 1 };
    return c;
}

static void set_ab(bool a, bool b)
{
    SIO_GPIO_IN = (SIO_GPIO_IN & ~((1UL << 20) | (1UL << 21)))
            | (a ? (1UL << 20) : 0U) | (b ? (1UL << 21) : 0U);
}

static void test_cw_rotation_counts_up(void)
{
    quad_enc_t dev;
    quad_enc_config_t c = ky040_cfg();
    TEST_ASSERT_EQUAL(QUAD_ENC_STATUS_OK, quad_enc_init(&dev, &c));
    /* CW Gray cycle: 00 -> 01 -> 11 -> 10 = +3 steps */
    set_ab(false, false); quad_enc_sample(&dev);
    set_ab(false, true);  quad_enc_sample(&dev);
    set_ab(true, true);   quad_enc_sample(&dev);
    set_ab(true, false);  quad_enc_sample(&dev);
    TEST_ASSERT_EQUAL_INT32(3, quad_enc_count(&dev));
}

static void test_ccw_rotation_counts_down(void)
{
    quad_enc_t dev;
    quad_enc_config_t c = ky040_cfg();
    quad_enc_init(&dev, &c);
    /* reverse cycle: 00 -> 10 -> 11 -> 01 = -3 */
    set_ab(false, false); quad_enc_sample(&dev);
    set_ab(true, false);  quad_enc_sample(&dev);
    set_ab(true, true);   quad_enc_sample(&dev);
    set_ab(false, true);  quad_enc_sample(&dev);
    TEST_ASSERT_EQUAL_INT32(-3, quad_enc_count(&dev));
}

static void test_two_bit_jump_rejected_as_noise(void)
{
    quad_enc_t dev;
    quad_enc_config_t c = ky040_cfg();
    quad_enc_init(&dev, &c);
    set_ab(false, false); quad_enc_sample(&dev);
    set_ab(true, true);   quad_enc_sample(&dev);   /* impossible jump */
    TEST_ASSERT_EQUAL_INT32(0, quad_enc_count(&dev));
    /* recovery: valid stepping continues from wherever the pins settled */
    set_ab(true, false);  quad_enc_sample(&dev);
    TEST_ASSERT_EQUAL_INT32(1, quad_enc_count(&dev));
}

static void test_detents_are_four_step_groups(void)
{
    quad_enc_t dev;
    quad_enc_config_t c = ky040_cfg();
    int i;
    quad_enc_init(&dev, &c);
    set_ab(false, false); quad_enc_sample(&dev);
    for (i = 0; i < 2; i++) {   /* two full detent cycles = 8 steps */
        set_ab(false, true);  quad_enc_sample(&dev);
        set_ab(true, true);   quad_enc_sample(&dev);
        set_ab(true, false);  quad_enc_sample(&dev);
        set_ab(false, false); quad_enc_sample(&dev);
    }
    TEST_ASSERT_EQUAL_INT32(8, quad_enc_count(&dev));
    TEST_ASSERT_EQUAL_INT32(2, quad_enc_detents(&dev));
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_cw_rotation_counts_up);
    RUN_TEST(test_ccw_rotation_counts_down);
    RUN_TEST(test_two_bit_jump_rejected_as_noise);
    RUN_TEST(test_detents_are_four_step_groups);
    return UNITY_END();
}
#endif
