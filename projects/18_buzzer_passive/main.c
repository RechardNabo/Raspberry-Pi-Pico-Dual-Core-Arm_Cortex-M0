/* main.c — kit module #18 Passive Buzzer — standalone firmware, RP2040.
 * Non-blocking super-loop over the device layer. Wiring/verification: README.md.
 * Target build: Pico SDK. Host-verified logic: drivers/tests/unit/. */
#include <stdint.h>
#include <stdbool.h>

#include "../../drivers/devices/kit/kit.h"
#include "../../drivers/peripherals/gpio/rp_gpio.h"
#include "../../drivers/peripherals/timer/rp_timer.h"

#define TICK_US  300000UL
#define LED_PIN  25U   /* Pico onboard LED — the universal demo indicator */

int main(void)
{
    rp_timer_init();
    buzzer_t buzz;
    buzzer_init(&buzz, 3U, 0U, 125000000UL);
    /* two octaves of C major in integer Hz */
    static const uint32_t scale[15] = {
        262U, 294U, 330U, 349U, 392U, 440U, 494U,
        523U, 587U, 659U, 698U, 784U, 880U, 988U, 1047U };
    uint8_t note = 0U;

    for (;;) {
        const uint64_t tick_start = rp_timer_now_us();
        buzzer_tone(&buzz, scale[note]);
        note = (uint8_t)((note + 1U) % 15U);
        while ((rp_timer_now_us() - tick_start) < TICK_US) { }
    }
}
