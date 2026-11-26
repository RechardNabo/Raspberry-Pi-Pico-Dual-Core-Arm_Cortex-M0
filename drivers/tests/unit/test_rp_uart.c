/* test_rp_uart.c — PL011 divisor math + LCR_H assembly, bit-exact. */
#include "unity.h"
#include "xc.h"
#include "../../peripherals/uart/rp_uart.h"

void setUp(void) { mock_sfr_reset(); }
void tearDown(void) {}

static rp_uart_config_t cfg115200(void)
{
    rp_uart_config_t c;
    c.clk_hz = 125000000UL;
    c.baud = 115200UL;
    c.parity = RP_UART_PARITY_NONE;
    c.stop_bits = 1U;
    return c;
}

static void test_divisor_115200_at_125mhz(void)
{
    uint32_t ibrd, fbrd;
    rp_uart_divisor(125000000UL, 115200UL, &ibrd, &fbrd);
    /* d64 = 125e6*4/115200 = 4340.3 -> ibrd 67 fbrd 52 */
    TEST_ASSERT_EQUAL_UINT32(67U, ibrd);
    TEST_ASSERT_EQUAL_UINT32(52U, fbrd);
}

static void test_init_order_lcrh_before_enable(void)
{
    rp_uart_config_t c = cfg115200();
    TEST_ASSERT_EQUAL(RP_UART_STATUS_OK, rp_uart_init(RP_UART0, &c));
    TEST_ASSERT_EQUAL_HEX32(67U, U0IBRD);
    TEST_ASSERT_EQUAL_HEX32(52U, U0FBRD);
    /* 8N1 + FIFO enable: wlen8 bits6:5=3, fen bit4 */
    TEST_ASSERT_EQUAL_HEX32((3UL << 5) | (1UL << 4), U0LCRH);
    /* UARTEN + TXE + RXE */
    TEST_ASSERT_EQUAL_HEX32(1U | (1UL << 8) | (1UL << 9), U0CR);
}

static void test_even_parity_two_stop(void)
{
    rp_uart_config_t c = cfg115200();
    c.parity = RP_UART_PARITY_EVEN;
    c.stop_bits = 2U;
    rp_uart_init(RP_UART1, &c);
    TEST_ASSERT_EQUAL_HEX32((3UL << 5) | (1UL << 4) | (1UL << 1) | (1UL << 2) | (1UL << 3), U1LCRH);
}

static void test_write_gated_by_txff(void)
{
    rp_uart_config_t c = cfg115200();
    rp_uart_init(RP_UART0, &c);
    U0FR = 1UL << 5;
    TEST_ASSERT_FALSE(rp_uart_write_byte(RP_UART0, 0x55U));
    U0FR = 0U;
    TEST_ASSERT_TRUE(rp_uart_write_byte(RP_UART0, 0x55U));
    TEST_ASSERT_EQUAL_HEX32(0x55U, U0DR);
}

static void test_read_gated_by_rxfe(void)
{
    uint8_t out = 0U;
    U1FR = 1UL << 4;   /* rx fifo empty */
    TEST_ASSERT_FALSE(rp_uart_read_byte(RP_UART1, &out));
    U1FR = 0U;
    U1DR = 0x5AU;
    TEST_ASSERT_TRUE(rp_uart_read_byte(RP_UART1, &out));
    TEST_ASSERT_EQUAL_HEX8(0x5AU, out);
}

#ifndef UNIT_TEST_NO_MAIN
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_divisor_115200_at_125mhz);
    RUN_TEST(test_init_order_lcrh_before_enable);
    RUN_TEST(test_even_parity_two_stop);
    RUN_TEST(test_write_gated_by_txff);
    RUN_TEST(test_read_gated_by_rxfe);
    return UNITY_END();
}
#endif
