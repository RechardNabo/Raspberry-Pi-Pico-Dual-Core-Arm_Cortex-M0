/* rp_timer.h — microsecond timer driver for RP2040.
 * The chip has a free-running 64-bit 1 MHz timer. THE trap: reading the
 * 64-bit value is not atomic — the low read LATCHES the high bits, so the
 * correct order is low-then-high. Reading high-then-low tears across a
 * 32-bit rollover. Driver exists to make the correct order the only
 * order the application can express. */
#ifndef RP_TIMER_H
#define RP_TIMER_H

#include <stdint.h>

void rp_timer_init(void);
uint64_t rp_timer_now_us(void);
void rp_timer_delay_us(uint32_t us);
uint32_t rp_timer_elapsed_us(uint64_t since);

#endif
