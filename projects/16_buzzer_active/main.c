/* main.c — kit module #16 Active Buzzer — standalone firmware, RP2040.
 * Non-blocking super-loop over the device layer. Wiring/verification: README.md.
 * Target build: Pico SDK. Host-verified logic: drivers/tests/unit/. */
#include <stdint.h>
#include <stdbool.h>

#include "../../drivers/devices/kit/kit.h"
#include "../../drivers/peripherals/gpio/rp_gpio.h"
#include "../../drivers/peripherals/timer/rp_timer.h"

#define TICK_US  100000UL
#define LED_PIN  25U   /* Pico onboard LED — the universal demo indicator */

int main(void)
{
    rp_timer_init();
    dig_out_t buzzer;
    dig_out_config_t cfg = kit_dig_out_config(KIT_BUZZER_ACTIVE, 17U);
    dig_out_init(&buzzer, &cfg);
    /* SOS pattern: 3 short, 3 long, 3 short — one element per tick group */
    static const uint8_t sos[18] = { 1,0, 1,0, 1,0,0, 1,1,1,0, 1,1,1,0, 1,1,1 };
    uint8_t step = 0U;

    for (;;) {
        const uint64_t tick_start = rp_timer_now_us();
        dig_out_set(&buzzer, sos[step] != 0U);
        step = (uint8_t)((step + 1U) % 18U);
        while ((rp_timer_now_us() - tick_start) < TICK_US) { }
    }
}
