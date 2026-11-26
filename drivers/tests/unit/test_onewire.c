/* test_onewire.c — bus primitives + protocol math against the time engine. */
#include "unity.h"
#include "xc.h"
#include "../../devices/onewire/onewire.h"
#include "../../peripherals/timer/rp_timer.h"

#define PIN 22U

void setUp(void) { mock_sfr_reset(); rp_timer_init(); }
void tearDown(void) {}

static void test_reset_detects_presence_pulse(void)
{
    ow_bus_t bus;
    TEST_ASSERT_EQUAL(ONEWIRE_STATUS_OK, ow_bus_init(&bus, PIN));
    SIO_GPIO_IN &= ~(1UL << PIN);   /* device pulls low: present */
    TEST_ASSERT_EQUAL(ONEWIRE_STATUS_OK, ow_reset(&bus));
}

static void test_reset_no_device(void)
{
    ow_bus_t bus;
    ow_bus_init(&bus, PIN);
    SIO_GPIO_IN |= (1UL << PIN);    /* bus stays high: nobody home */
    TEST_ASSERT_EQUAL(ONEWIRE_STATUS_NO_PRESENCE, ow_reset(&bus));
}

static void test_reset_timing_budget(void)
{
    ow_bus_t bus;
    uint64_t start;
    ow_bus_init(&bus, PIN);
    SIO_GPIO_IN &= ~(1UL << PIN);
    start = mock_time_now();
    ow_reset(&bus);
    /* 480 low + 70 sample + 410 recovery = ~960us minimum */
    TEST_ASSERT_TRUE(mock_time_now() - start >= 950U);
}

static void test_ds18b20_millic_conversion(void)
{
    TEST_ASSERT_EQUAL_INT32(25000, ds18b20_raw_to_millic((int16_t)0x0190));   /* 25.000C */
    TEST_ASSERT_EQUAL_INT32(-12500, ds18b20_raw_to_millic((int16_t)0xFF38));  /* -12.500C */
    TEST_ASSERT_EQUAL_INT32(0, ds18b20_raw_to_millic(0));
}

static void test_read_bit_sampling(void)
{
    ow_bus_t bus;
    ow_bus_init(&bus, PIN);
    SIO_GPIO_IN |= (1UL << PIN);    /* device releases: reads 1 at sample point */
    TEST_ASSERT_TRUE(ow_read_bit(&bus));
}

static void test_dht11_bit1_long_pulse_decodes_one(void)
{
    dht11_t dev;
    bool bit = false;
    dht11_init(&dev, PIN);
    SIO_GPIO_IN &= ~(1UL << PIN);          /* in the low phase */
    mock_time_schedule(20U, PIN, true);   /* low ends at +20us */
    mock_time_schedule(90U, PIN, false);  /* high ends at +90us -> 70us pulse = 1 */
    TEST_ASSERT_EQUAL(ONEWIRE_STATUS_OK, dht11_read_bit(&dev, &bit));
    TEST_ASSERT_TRUE(bit);
}

static void test_dht11_bit0_short_pulse_decodes_zero(void)
{
    dht11_t dev;
    bool bit = true;
    dht11_init(&dev, PIN);
    SIO_GPIO_IN &= ~(1UL << PIN);
    mock_time_schedule(20U, PIN, true);
    mock_time_schedule(48U, PIN, false);  /* 28us pulse = 0 */
    TEST_ASSERT_EQUAL(ONEWIRE_STATUS_OK, dht11_read_bit(&dev, &bit));
    TEST_ASSERT_FALSE(bit);
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_reset_detects_presence_pulse);
    RUN_TEST(test_reset_no_device);
    RUN_TEST(test_reset_timing_budget);
    RUN_TEST(test_ds18b20_millic_conversion);
    RUN_TEST(test_read_bit_sampling);
    RUN_TEST(test_dht11_bit1_long_pulse_decodes_one);
    RUN_TEST(test_dht11_bit0_short_pulse_decodes_zero);
    return UNITY_END();
}
#endif
