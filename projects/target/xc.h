/* xc.h — TARGET register mapping for RP2040 (host-mock counterpart:
 *   drivers/tests/unit/support/xc.h).
 * Flat names mapped to silicon addresses per the RP2040 datasheet §2.x
 * peripheral register maps. SIO atomic aliases are real hardware aliases. */
#ifndef XC_H
#define XC_H

#include <stdint.h>
#include <stdbool.h>

#define REG32(addr) (*(volatile uint32_t *)(addr))

/* SIO — single-cycle IO block, base 0xD0000000 (datasheet §2.3.1) */
#define SIO_BASE        0xD0000000UL
#define SIO_GPIO_IN     REG32(SIO_BASE + 0x004UL)
#define SIO_GPIO_OUT    REG32(SIO_BASE + 0x010UL)
#define SIO_GPIO_OUT_SET REG32(SIO_BASE + 0x014UL)
#define SIO_GPIO_OUT_CLR REG32(SIO_BASE + 0x018UL)
#define SIO_GPIO_OUT_XOR REG32(SIO_BASE + 0x01CUL)
#define SIO_GPIO_OE     REG32(SIO_BASE + 0x020UL)
#define SIO_GPIO_OE_SET REG32(SIO_BASE + 0x024UL)
#define SIO_GPIO_OE_CLR REG32(SIO_BASE + 0x028UL)

/* UART0/1 — PL011, bases 0x40034000 / 0x40038000 (datasheet §4.2.8) */
#define U0DR   REG32(0x40034000UL + 0x000UL)
#define U0FR   REG32(0x40034000UL + 0x018UL)
#define U0IBRD REG32(0x40034000UL + 0x024UL)
#define U0FBRD REG32(0x40034000UL + 0x028UL)
#define U0LCRH REG32(0x40034000UL + 0x02CUL)
#define U0CR   REG32(0x40034000UL + 0x030UL)
#define U1DR   REG32(0x40038000UL + 0x000UL)
#define U1FR   REG32(0x40038000UL + 0x018UL)
#define U1IBRD REG32(0x40038000UL + 0x024UL)
#define U1FBRD REG32(0x40038000UL + 0x028UL)
#define U1LCRH REG32(0x40038000UL + 0x02CUL)
#define U1CR   REG32(0x40038000UL + 0x030UL)

/* SPI0/1 — PL022, bases 0x4003C000 / 0x40040000 (datasheet §4.4.14) */
#define SPI0CR0  REG32(0x4003C000UL + 0x000UL)
#define SPI0CR1  REG32(0x4003C000UL + 0x004UL)
#define SPI0DR   REG32(0x4003C000UL + 0x008UL)
#define SPI0SR   REG32(0x4003C000UL + 0x00CUL)
#define SPI0CPSR REG32(0x4003C000UL + 0x010UL)
#define SPI1CR0  REG32(0x40040000UL + 0x000UL)
#define SPI1CR1  REG32(0x40040000UL + 0x004UL)
#define SPI1DR   REG32(0x40040000UL + 0x008UL)
#define SPI1SR   REG32(0x40040000UL + 0x00CUL)
#define SPI1CPSR REG32(0x40040000UL + 0x010UL)

/* I2C0/1 — DW-apb-i2c, bases 0x40044000 / 0x40048000 (datasheet §4.3.13) */
#define I2C0CON     REG32(0x40044000UL + 0x00UL)
#define I2C0TAR     REG32(0x40044000UL + 0x04UL)
#define I2C0DATACMD REG32(0x40044000UL + 0x10UL)
#define I2C0FSHCNT  REG32(0x40044000UL + 0x1CUL)
#define I2C0FSLCNT  REG32(0x40044000UL + 0x20UL)
#define I2C0ENABLE  REG32(0x40044000UL + 0x6CUL)
#define I2C0STATUS  REG32(0x40044000UL + 0x70UL)
#define I2C1CON     REG32(0x40048000UL + 0x00UL)
#define I2C1TAR     REG32(0x40048000UL + 0x04UL)
#define I2C1DATACMD REG32(0x40048000UL + 0x10UL)
#define I2C1FSHCNT  REG32(0x40048000UL + 0x1CUL)
#define I2C1FSLCNT  REG32(0x40048000UL + 0x20UL)
#define I2C1ENABLE  REG32(0x40048000UL + 0x6CUL)
#define I2C1STATUS  REG32(0x40048000UL + 0x70UL)

/* PWM — base 0x40050000, 8 slices, stride 0x14 (datasheet §4.5.3) */
#define PWM_BASE    0x40050000UL
#define PWM_STRIDE  0x14UL
#define PWM_CSR     ((volatile uint32_t *)PWM_BASE)                       /* +0x00 per slice */
#define PWM_DIV     ((volatile uint32_t *)(PWM_BASE + 0x04UL))            /* +0x04 */
#define PWM_CC      ((volatile uint32_t *)(PWM_BASE + 0x0CUL))            /* +0x0C */
#define PWM_TOP     ((volatile uint32_t *)(PWM_BASE + 0x10UL))            /* +0x10 */
/* NOTE: silicon interleaves A/B channels per slice — arrays here index by
 * slice*stride; driver handles the stride. See pwm driver target notes. */

/* ADC — base 0x4004C000 (datasheet §4.9.6) */
#define ADC_CS     REG32(0x4004C000UL + 0x00UL)
#define ADC_RESULT REG32(0x4004C000UL + 0x04UL)

/* TIMER — base 0x40054000 (datasheet §4.6.5) */
#define TIMERAWH   REG32(0x40054000UL + 0x00UL)
#define TIMERAWL   REG32(0x40054000UL + 0x04UL)

#endif
