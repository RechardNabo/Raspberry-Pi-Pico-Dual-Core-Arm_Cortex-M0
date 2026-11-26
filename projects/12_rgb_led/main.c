/* main.c — Continuous rainbow rotation.
 * Standalone flashable firmware for this module. Hardware binding lives
 * here; ALL application logic lives in rgb_led_app.c and is pinned by the
 * project's 3 host tests (see test_rgb_led_app.c, run: cd projects && make). */
#include <stdint.h>
#include "rgb_led_app.h"
#include "../../drivers/devices/kit/kit.h"

int main(void)
{
    rgb_led_app_t app;
    rgb_led_app_init(&app, 1U, 0U, 2U, 0U, 3U, 0U, 125000000U, 1000U);
    for (;;) {
        rgb_led_app_step(&app);
        /* 10ms tick: full rainbow cycle in 7.7s */
    }
    return 0;
}
