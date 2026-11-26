/* main.c — Two-octave scale player.
 * Standalone flashable firmware for this module. Hardware binding lives
 * here; ALL application logic lives in buzzer_passive_app.c and is pinned by the
 * project's 3 host tests (see test_buzzer_passive_app.c, run: cd projects && make). */
#include <stdint.h>
#include "buzzer_passive_app.h"
#include "../../drivers/devices/kit/kit.h"

int main(void)
{
    buzzer_passive_app_t app;
    buzzer_passive_app_init(&app, 0U, 1U, 125000000U);
    for (;;) {
        buzzer_passive_app_step(&app);
        /* one note per 300ms tick on target */
    }
    return 0;
}
