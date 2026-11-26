/* main.c — kit module #15 KY-040 Rotary Encoder — standalone firmware, RP2040.
 * Non-blocking super-loop over the device layer. Wiring/verification: README.md.
 * Target build: Pico SDK. Host-verified logic: drivers/tests/unit/. */
#include <stdint.h>
#include <stdbool.h>

#include "../../drivers/devices/kit/kit.h"
#include "../../drivers/peripherals/gpio/rp_gpio.h"
#include "../../drivers/peripherals/timer/rp_timer.h"

#define TICK_US  2000UL
#define LED_PIN  25U   /* Pico onboard LED — the universal demo indicator */

int main(void)
{
    rp_timer_init();
    quad_enc_t enc;
    quad_enc_config_t ecfg = { 6U, 7U, 1 };
    quad_enc_init(&enc, &ecfg);
    dig_in_t btn;
    dig_in_config_t bcfg = { 8U, DIG_IN_ACTIVE_LOW, 5U };
    dig_in_init(&btn, &bcfg);
    int32_t last_detents = 0;

    for (;;) {
        const uint64_t tick_start = rp_timer_now_us();
        quad_enc_sample(&enc);
        dig_in_sample(&btn);
        if (dig_in_state(&btn)) {
            quad_enc_reset(&enc);
        }
        if (quad_enc_detents(&enc) != last_detents) {
            last_detents = quad_enc_detents(&enc);
            /* report position over UART console */
        }
        while ((rp_timer_now_us() - tick_start) < TICK_US) { }
    }
}
