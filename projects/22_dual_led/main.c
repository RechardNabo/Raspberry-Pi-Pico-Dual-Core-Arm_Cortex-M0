/* main.c — kit module #22 Dual Red/Green LED — standalone firmware, RP2040.
 * Non-blocking super-loop over the device layer. Wiring/verification: README.md.
 * Target build: Pico SDK. Host-verified logic: drivers/tests/unit/. */
#include <stdint.h>
#include <stdbool.h>

#include "../../drivers/devices/kit/kit.h"
#include "../../drivers/peripherals/gpio/rp_gpio.h"
#include "../../drivers/peripherals/timer/rp_timer.h"

#define TICK_US  500000UL
#define LED_PIN  25U   /* Pico onboard LED — the universal demo indicator */

int main(void)
{
    rp_timer_init();
    dig_out_t red, green;
    dig_out_config_t rcfg = { 14U, DIG_OUT_ACTIVE_HIGH };
    dig_out_config_t gcfg = { 15U, DIG_OUT_ACTIVE_HIGH };
    dig_out_init(&red, &rcfg);
    dig_out_init(&green, &gcfg);
    bool phase = false;

    for (;;) {
        const uint64_t tick_start = rp_timer_now_us();
        phase = !phase;
        dig_out_set(&red, phase);
        dig_out_set(&green, !phase);   /* strict alternation, never both */
        while ((rp_timer_now_us() - tick_start) < TICK_US) { }
    }
}
