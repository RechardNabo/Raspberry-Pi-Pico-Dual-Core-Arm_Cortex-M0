/* rp_spi.h — SPI master driver for RP2040 (SPI0/1, PL022-derived).
 * Bitrate = clk_peri / (CPSDVSR x (1 + SCR)) — TWO cascaded divisors,
 * CPSDVSR even 2-254. The driver picks the closest combination that
 * never exceeds the target clock. */
#ifndef RP_SPI_H
#define RP_SPI_H

#include <stdint.h>
#include <stdbool.h>

typedef enum { RP_SPI0 = 0, RP_SPI1, RP_SPI_CHANNEL_COUNT } rp_spi_channel_t;
typedef enum {
    RP_SPI_STATUS_OK = 0, RP_SPI_STATUS_INVALID_CHANNEL,
    RP_SPI_STATUS_INVALID_PARAM, RP_SPI_STATUS_INVALID_CLOCK
} rp_spi_status_t;
typedef enum { RP_SPI_MODE_0 = 0, RP_SPI_MODE_1, RP_SPI_MODE_2, RP_SPI_MODE_3 } rp_spi_mode_t;

typedef struct { uint32_t clk_hz; uint32_t clock_hz; rp_spi_mode_t mode; } rp_spi_config_t;

rp_spi_status_t rp_spi_init(rp_spi_channel_t ch, const rp_spi_config_t *config);
rp_spi_status_t rp_spi_exchange_byte(rp_spi_channel_t ch, uint8_t tx, uint8_t *rx_out);
void rp_spi_divisors(uint32_t clk_hz, uint32_t clock_hz, uint32_t *cpsdvsr, uint32_t *scr);

#endif
