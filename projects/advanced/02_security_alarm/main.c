/* main.c — security alarm binding: pins from the kit table. */
#include "security_alarm_app.h"
#include "../../../drivers/devices/kit/kit.h"

#define TICK_LOOPS 20000UL

int main(void)
{
    static security_alarm_app_t app;
    security_alarm_app_init(&app,
                            kit_default_pin(KIT_VIBRATION),
                            kit_default_pin(KIT_BUTTON),
                            kit_default_pin(KIT_BUZZER_ACTIVE),
                            kit_default_pin(KIT_DUAL_LED));
    for (;;) {
        security_alarm_app_step(&app);
        for (volatile uint32_t i = 0; i < TICK_LOOPS; i++) { }
    }
}
