/* main.c — climate guard: LM35 thermostat + overheat alarm on hardware.
 * Flashable binding: pins come from the kit table, app logic is tested. */
#include "climate_guard_app.h"
#include "../../../drivers/devices/kit/kit.h"

#define TICK_LOOPS 20000UL

int main(void)
{
    static climate_guard_app_t app;
    climate_guard_app_init(&app, 0U,
                           kit_default_pin(KIT_RELAY),
                           kit_default_pin(KIT_BUZZER_ACTIVE));
    for (;;) {
        climate_guard_app_step(&app);
        for (volatile uint32_t i = 0; i < TICK_LOOPS; i++) { }
    }
}
