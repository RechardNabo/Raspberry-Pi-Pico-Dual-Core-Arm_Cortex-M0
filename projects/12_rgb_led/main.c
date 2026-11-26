/* main.c — kit module #12 RGB LED — standalone firmware, RP2040.
 * Non-blocking super-loop over the device layer. Wiring/verification: README.md.
 * Target build: Pico SDK. Host-verified logic: drivers/tests/unit/. */
#include <stdint.h>
#include <stdbool.h>

#include "../../drivers/devices/kit/kit.h"
#include "../../drivers/peripherals/gpio/rp_gpio.h"
#include "../../drivers/peripherals/timer/rp_timer.h"

#define TICK_US  20000UL
#define LED_PIN  25U   /* Pico onboard LED — the universal demo indicator */

int main(void)
{
    rp_timer_init();
    rgb_led_t rgb;
    rgb_led_init(&rgb, 0U, 0U, 1U, 0U, 2U, 0U, 125000000UL, 1000UL);
    uint16_t hue = 0U;   /* 0-767 wheel position: 256 per color transition */

    for (;;) {
        const uint64_t tick_start = rp_timer_now_us();
        /* three-phase color wheel: R->G->B->R through 768 steps */
        const uint16_t h = hue % 768U;
        uint8_t r, g, b;
        if (h < 256U)       { r = (uint8_t)(255U - h); g = (uint8_t)h; b = 0U; }
        else if (h < 512U)  { r = 0U; g = (uint8_t)(511U - h); b = (uint8_t)(h - 256U); }
        else                { r = (uint8_t)(h - 512U); g = 0U; b = (uint8_t)(767U - h); }
        rgb_led_set(&rgb, r, g, b);
        hue++;
        while ((rp_timer_now_us() - tick_start) < TICK_US) { }
    }
}
