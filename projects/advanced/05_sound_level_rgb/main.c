/* main.c — sound level meter binding. */
#include "sound_level_rgb_app.h"
#include "../../../drivers/devices/kit/kit.h"

#define TICK_LOOPS 20000UL

int main(void)
{
    static sound_level_rgb_app_t app;
    sound_level_rgb_app_init(&app, 0U, 3U, 4U, 5U);
    for (;;) {
        sound_level_rgb_app_step(&app);
        for (volatile uint32_t i = 0; i < TICK_LOOPS; i++) { }
    }
}
