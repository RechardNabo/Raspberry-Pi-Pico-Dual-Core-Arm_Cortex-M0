/* main.c — kit module #6 5V Relay — standalone firmware, RP2040.
 * Non-blocking super-loop over the device layer. Wiring/verification: README.md.
 * Target build: Pico SDK. Host-verified logic: drivers/tests/unit/. */
#include <stdint.h>
#include <stdbool.h>

#include "../../drivers/devices/kit/kit.h"
#include "../../drivers/peripherals/gpio/rp_gpio.h"
#include "../../drivers/peripherals/timer/rp_timer.h"

#define TICK_US  1000000UL
#define LED_PIN  25U   /* Pico onboard LED — the universal demo indicator */

int main(void)
{
    rp_timer_init();
    dig_out_t relay;
    dig_out_config_t cfg = kit_dig_out_config(KIT_RELAY, 16U);  /* ACTIVE-LOW fact baked in */
    dig_out_init(&relay, &cfg);   /* coil guaranteed OFF from power-on */
    bool phase = false;

    for (;;) {
        const uint64_t tick_start = rp_timer_now_us();
        phase = !phase;
        dig_out_set(&relay, phase);   /* 0.5Hz click cadence */
        while ((rp_timer_now_us() - tick_start) < TICK_US) { }
    }
}
