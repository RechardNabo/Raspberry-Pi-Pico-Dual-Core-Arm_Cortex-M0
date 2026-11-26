/* onewire.c — see onewire.h. Timing per DS18B20/DHT11 datasheets:
 * reset low 480us, presence window 15-60us after release, presence
 * pulse 60-240us. Write slot 60us min: 1 = release after <15us low,
 * 0 = hold low whole slot. Read slot: pull low 1us, release, sample
 * after 15us. Open-drain emulation: output-low drives, input releases
 * (external pull-up on the module). */
#include "onewire.h"
#include "../../peripherals/gpio/rp_gpio.h"
#include "../../bsp/rp_board.h"
#include "../../peripherals/timer/rp_timer.h"

static void drive_low(ow_bus_t *bus)
{
    rp_gpio_set_output(bus->pin);
    rp_gpio_write(bus->pin, false);
}

static void release(ow_bus_t *bus)
{
    rp_gpio_set_input(bus->pin);   /* pull-up on module takes it high */
}

onewire_status_t ow_bus_init(ow_bus_t *bus, uint8_t pin)
{
    if (bus == 0) return ONEWIRE_STATUS_INVALID_CONFIG;
    if (!rp_gpio_valid(pin)) return ONEWIRE_STATUS_INVALID_CONFIG;
    bus->pin = pin;
    release(bus);
    return ONEWIRE_STATUS_OK;
}

onewire_status_t ow_reset(ow_bus_t *bus)
{
    uint64_t start;
    bool present = false;
    drive_low(bus);
    rp_timer_delay_us(480U);
    release(bus);
    rp_timer_delay_us(70U);          /* presence window center */
    present = !rp_gpio_read(bus->pin);
    start = rp_timer_now_us();
    while ((rp_timer_now_us() - start) < 410U) { /* slot recovery */ }
    return present ? ONEWIRE_STATUS_OK : ONEWIRE_STATUS_NO_PRESENCE;
}

void ow_write_bit(ow_bus_t *bus, bool bit)
{
    drive_low(bus);
    if (bit) {
        rp_timer_delay_us(6U);       /* release fast for a 1 */
        release(bus);
        rp_timer_delay_us(64U);
    } else {
        rp_timer_delay_us(60U);      /* hold low whole slot for a 0 */
        release(bus);
        rp_timer_delay_us(10U);
    }
}

bool ow_read_bit(ow_bus_t *bus)
{
    bool bit;
    drive_low(bus);
    rp_timer_delay_us(2U);
    release(bus);
    rp_timer_delay_us(11U);          /* sample point ~13us into slot */
    bit = rp_gpio_read(bus->pin);
    rp_timer_delay_us(55U);
    return bit;
}

void ow_write_byte(ow_bus_t *bus, uint8_t byte)
{
    uint8_t i;
    for (i = 0U; i < 8U; i++) {
        ow_write_bit(bus, (byte & (1U << i)) != 0U);   /* LSB first per spec */
    }
}

uint8_t ow_read_byte(ow_bus_t *bus)
{
    uint8_t i, byte = 0U;
    for (i = 0U; i < 8U; i++) {
        if (ow_read_bit(bus)) byte |= (uint8_t)(1U << i);
    }
    return byte;
}

/* ---- DS18B20 ---- */

onewire_status_t ds18b20_init(ds18b20_t *dev, uint8_t pin)
{
    if (dev == 0) return ONEWIRE_STATUS_INVALID_CONFIG;
    return ow_bus_init(&dev->bus, pin);
}

onewire_status_t ds18b20_start_conversion(ds18b20_t *dev)
{
    onewire_status_t st = ow_reset(&dev->bus);
    if (st != ONEWIRE_STATUS_OK) return st;
    ow_write_byte(&dev->bus, 0xCCU);   /* SKIP ROM — single device on bus */
    ow_write_byte(&dev->bus, 0x44U);   /* CONVERT T */
    return ONEWIRE_STATUS_OK;
}

onewire_status_t ds18b20_read_scratchpad(ds18b20_t *dev, uint8_t *buf9)
{
    uint8_t i;
    onewire_status_t st = ow_reset(&dev->bus);
    if (st != ONEWIRE_STATUS_OK) return st;
    ow_write_byte(&dev->bus, 0xCCU);
    ow_write_byte(&dev->bus, 0xBEU);   /* READ SCRATCHPAD */
    for (i = 0U; i < 9U; i++) {
        buf9[i] = ow_read_byte(&dev->bus);
    }
    return ONEWIRE_STATUS_OK;
}

int32_t ds18b20_raw_to_millic(int16_t raw)
{
    /* 12-bit resolution: LSB = 1/16 C — millidegree = raw x 1000 / 16 */
    return ((int32_t)raw * 1000) / 16;
}

/* ---- DHT11 ---- */

onewire_status_t dht11_init(dht11_t *dev, uint8_t pin)
{
    if (dev == 0) return ONEWIRE_STATUS_INVALID_CONFIG;
    return ow_bus_init(&dev->bus, pin);
}

onewire_status_t dht11_read_bit(dht11_t *dev, bool *bit_out)
{
    /* bit frame: ~50us low, then 26-28us high (0) or ~70us high (1).
     * Caller synchronizes to edges; this primitive measures one high
     * pulse width after the low phase. */
    uint64_t start = rp_timer_now_us();
    while (!rp_gpio_read(dev->bus.pin)) {              /* wait out the low */
        if ((rp_timer_now_us() - start) > 200U) return ONEWIRE_STATUS_TIMEOUT;
    }
    start = rp_timer_now_us();
    while (rp_gpio_read(dev->bus.pin)) {               /* measure the high */
        if ((rp_timer_now_us() - start) > 200U) return ONEWIRE_STATUS_TIMEOUT;
    }
    *bit_out = (rp_timer_now_us() - start) > 40U;
    return ONEWIRE_STATUS_OK;
}

onewire_status_t dht11_read(dht11_t *dev, uint8_t *rh_out, uint8_t *temp_out)
{
    uint8_t data[5] = { 0U, 0U, 0U, 0U, 0U };
    uint8_t i;
    int bit;
    uint64_t start;

    /* start signal: 18ms low, release, wait for sensor response */
    drive_low(&dev->bus);
    rp_timer_delay_us(18000U);
    release(&dev->bus);
    rp_timer_delay_us(40U);

    start = rp_timer_now_us();
    while (!rp_gpio_read(dev->bus.pin)) {              /* sensor pull-low ack */
        if ((rp_timer_now_us() - start) > 200U) return ONEWIRE_STATUS_NO_PRESENCE;
    }
    /* 40 bits: humidity int, humidity dec, temp int, temp dec, checksum */
    for (i = 0U; i < 5U; i++) {
        for (bit = 7; bit >= 0; bit--) {
            bool b = false;
            if (dht11_read_bit(dev, &b) != ONEWIRE_STATUS_OK)
                return ONEWIRE_STATUS_TIMEOUT;
            if (b) data[i] |= (uint8_t)(1U << bit);
        }
    }
    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4])
        return ONEWIRE_STATUS_CRC_MISMATCH;
    *rh_out = data[0];
    *temp_out = data[2];
    return ONEWIRE_STATUS_OK;
}
