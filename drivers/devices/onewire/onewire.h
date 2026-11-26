/* onewire.h — 1-wire-style bitbang device classes: DS18B20 (#19) and
 * DHT11 (#4). NOT the same protocol — DS18B20 is true Dallas 1-wire
 * (reset/presence, then bit slots), DHT11 is a simpler pulse-width
 * response with no presence phase. Shared primitive: microsecond GPIO
 * bitbang on the timer driver. Timing-critical sequences are primitive-
 * level testable: each primitive is one bus interaction. */
#ifndef ONEWIRE_H
#define ONEWIRE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    ONEWIRE_STATUS_OK = 0,
    ONEWIRE_STATUS_INVALID_CONFIG,
    ONEWIRE_STATUS_NO_PRESENCE,
    ONEWIRE_STATUS_TIMEOUT,
    ONEWIRE_STATUS_CRC_MISMATCH
} onewire_status_t;

/* ---- bus primitives (shared timing core) ---- */

typedef struct {
    uint8_t pin;
} ow_bus_t;

onewire_status_t ow_bus_init(ow_bus_t *bus, uint8_t pin);
/* DS18B20 primitives */
onewire_status_t ow_reset(ow_bus_t *bus);           /* presence pulse verified */
void ow_write_bit(ow_bus_t *bus, bool bit);
bool ow_read_bit(ow_bus_t *bus);
void ow_write_byte(ow_bus_t *bus, uint8_t byte);
uint8_t ow_read_byte(ow_bus_t *bus);

/* ---- DS18B20 (#19) ---- */

typedef struct {
    ow_bus_t bus;
} ds18b20_t;

onewire_status_t ds18b20_init(ds18b20_t *dev, uint8_t pin);
onewire_status_t ds18b20_start_conversion(ds18b20_t *dev);
onewire_status_t ds18b20_read_scratchpad(ds18b20_t *dev, uint8_t *buf9);
/* millidegree-C from the raw 16-bit temperature register — integer math */
int32_t ds18b20_raw_to_millic(int16_t raw);

/* ---- DHT11 (#4) ---- */

typedef struct {
    ow_bus_t bus;
} dht11_t;

onewire_status_t dht11_init(dht11_t *dev, uint8_t pin);
onewire_status_t dht11_read(dht11_t *dev, uint8_t *rh_out, uint8_t *temp_out);
/* one bit decode: measure high-pulse width; >40us = 1. Primitive-level. */
onewire_status_t dht11_read_bit(dht11_t *dev, bool *bit_out);

#endif
