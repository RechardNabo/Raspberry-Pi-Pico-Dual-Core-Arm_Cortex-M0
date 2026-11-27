/* main.c — fire sentinel binding. */
#include "fire_sentinel_app.h"
#include "../../../drivers/devices/kit/kit.h"

#define TICK_LOOPS 20000UL

int main(void)
{
    static fire_sentinel_app_t app;
    fire_sentinel_app_init(&app,
                           kit_default_pin(KIT_FLAME), 0U,
                           kit_default_pin(KIT_BUZZER_ACTIVE),
                           kit_default_pin(KIT_DUAL_LED));
    for (;;) {
        fire_sentinel_app_step(&app);
        for (volatile uint32_t i = 0; i < TICK_LOOPS; i++) { }
    }
}
