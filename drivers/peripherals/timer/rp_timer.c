/* rp_timer.c — see rp_timer.h. */
#include <xc.h>

#include "rp_timer.h"

void rp_timer_init(void)
{
    /* timer runs at 1 MHz out of reset — nothing to configure, but the
     * explicit init documents the contract point for other architectures */
}

uint64_t rp_timer_now_us(void)
{
    /* low-then-high: the TIMELR read latches TIMEHR atomically on silicon */
    const uint32_t lo = TIMERAWL;
    const uint32_t hi = TIMERAWH;
    return ((uint64_t)hi << 32) | lo;
}

void rp_timer_delay_us(uint32_t us)
{
    /* 64-bit subtraction is wrap-correct for the full timer lifetime */
    const uint64_t start = rp_timer_now_us();
    while ((rp_timer_now_us() - start) < us) {
        /* busy wait — bitbang protocols need deterministic timing */
    }
}

uint32_t rp_timer_elapsed_us(uint64_t since)
{
    return (uint32_t)(rp_timer_now_us() - since);
}
