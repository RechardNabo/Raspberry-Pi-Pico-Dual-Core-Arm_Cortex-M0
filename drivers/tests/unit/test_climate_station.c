/* test_climate_station.c — cross-validation logic + FSM LED output, bit-exact. */
#include "unity.h"
#include "xc.h"
#include "../../apps/climate_station/climate_station.h"
#include "../../peripherals/timer/rp_timer.h"

void setUp(void) { mock_sfr_reset(); rp_timer_init(); }
void tearDown(void) {}

static void test_band_classification(void)
{
    TEST_ASSERT_EQUAL(CLIMATE_BAND_COLD, climate_classify(1799));
    TEST_ASSERT_EQUAL(CLIMATE_BAND_COMFORT, climate_classify(1800));
    TEST_ASSERT_EQUAL(CLIMATE_BAND_COMFORT, climate_classify(2600));
    TEST_ASSERT_EQUAL(CLIMATE_BAND_HOT, climate_classify(2601));
}

static void test_crosscheck_tolerance(void)
{
    TEST_ASSERT_TRUE(climate_sensors_agree(2200, 2400));    /* 2C apart: ok */
    TEST_ASSERT_TRUE(climate_sensors_agree(2200, 2500));    /* exactly 3C: ok */
    TEST_ASSERT_FALSE(climate_sensors_agree(2200, 2501));   /* 3.01C: fault */
}

static void test_agreeing_sensors_produce_band_led(void)
{
    climate_station_t app;
    climate_station_init(&app);
    app.state = CLIMATE_STATE_EVALUATE;
    app.lm35_centic = 2200;
    app.ds18b20_centic = 2250;
    climate_station_step(&app);
    TEST_ASSERT_EQUAL(CLIMATE_BAND_COMFORT, app.band);
    TEST_ASSERT_EQUAL_UINT32(0U, app.fault_count);
    /* comfort = green: slice 1 (G) duty nonzero, R/B zero */
    TEST_ASSERT_TRUE((PWM_CC[1] & 0xFFFFU) > 0U);
    TEST_ASSERT_EQUAL_HEX32(0U, PWM_CC[0] & 0xFFFFU);
}

static void test_disagreeing_sensors_produce_fault(void)
{
    climate_station_t app;
    climate_station_init(&app);
    app.state = CLIMATE_STATE_EVALUATE;
    app.lm35_centic = 2200;
    app.ds18b20_centic = 2800;   /* 6C apart: one of them is lying */
    climate_station_step(&app);
    TEST_ASSERT_EQUAL(CLIMATE_BAND_FAULT, app.band);
    TEST_ASSERT_EQUAL_UINT32(1U, app.fault_count);
    /* violet: R and B nonzero, G zero */
    TEST_ASSERT_TRUE((PWM_CC[0] & 0xFFFFU) > 0U);
    TEST_ASSERT_TRUE((PWM_CC[2] & 0xFFFFU) > 0U);
    TEST_ASSERT_EQUAL_HEX32(0U, PWM_CC[1] & 0xFFFFU);
}

static void test_fsm_cycles_back_to_reading(void)
{
    climate_station_t app;
    climate_station_init(&app);
    app.state = CLIMATE_STATE_EVALUATE;
    app.lm35_centic = 2200;
    app.ds18b20_centic = 2200;
    climate_station_step(&app);
    TEST_ASSERT_EQUAL(CLIMATE_STATE_READ_LM35, app.state);
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_band_classification);
    RUN_TEST(test_crosscheck_tolerance);
    RUN_TEST(test_agreeing_sensors_produce_band_led);
    RUN_TEST(test_disagreeing_sensors_produce_fault);
    RUN_TEST(test_fsm_cycles_back_to_reading);
    return UNITY_END();
}
#endif
