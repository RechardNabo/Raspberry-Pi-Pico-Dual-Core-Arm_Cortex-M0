/* main.c — knock lock binding. */
#include "knock_lock_app.h"
#include "../../../drivers/devices/kit/kit.h"

#define TICK_LOOPS 20000UL

int main(void)
{
    static knock_lock_app_t app;
    knock_lock_app_init(&app,
                        kit_default_pin(KIT_KNOCK),
                        kit_default_pin(KIT_RELAY),
                        kit_default_pin(KIT_DUAL_LED));
    for (;;) {
        knock_lock_app_step(&app);
        for (volatile uint32_t i = 0; i < TICK_LOOPS; i++) { }
    }
}
