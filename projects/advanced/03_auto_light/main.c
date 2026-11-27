/* main.c — automatic lighting binding. */
#include "auto_light_app.h"
#include "../../../drivers/devices/kit/kit.h"

#define TICK_LOOPS 20000UL

int main(void)
{
    static auto_light_app_t app;
    auto_light_app_init(&app, 0U,
                        kit_default_pin(KIT_BUTTON),
                        kit_default_pin(KIT_RELAY));
    for (;;) {
        auto_light_app_step(&app);
        for (volatile uint32_t i = 0; i < TICK_LOOPS; i++) { }
    }
}
