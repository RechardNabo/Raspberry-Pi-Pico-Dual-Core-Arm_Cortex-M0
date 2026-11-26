/* main.c — kit module #1 PS2 Joystick — standalone firmware, RP2040.
 * Non-blocking super-loop over the device layer. Wiring/verification: README.md.
 * Target build: Pico SDK. Host-verified logic: drivers/tests/unit/. */
#include <stdint.h>
#include <stdbool.h>

#include "../../drivers/devices/kit/kit.h"
#include "../../drivers/peripherals/gpio/rp_gpio.h"
#include "../../drivers/peripherals/timer/rp_timer.h"

#define TICK_US  50000UL
#define LED_PIN  25U   /* Pico onboard LED — the universal demo indicator */

int main(void)
{
    rp_timer_init();
    analog_in_t vrx, vry;
    analog_in_config_t xcfg = kit_analog_config(KIT_JOYSTICK, 0U);
    analog_in_config_t ycfg = kit_analog_config(KIT_JOYSTICK, 1U);
    analog_in_init(&vrx, &xcfg);
    analog_in_init(&vry, &ycfg);
    rp_adc_init();
    dig_in_t btn;
    dig_in_config_t bcfg = kit_dig_in_config(KIT_JOYSTICK_BTN, 8U);
    dig_in_init(&btn, &bcfg);
    int32_t x = 0, y = 0;

    for (;;) {
        const uint64_t tick_start = rp_timer_now_us();
        analog_in_read_scaled(&vrx, &x);
        analog_in_read_scaled(&vry, &y);
        dig_in_sample(&btn);
        /* report x/y permille + button over UART console */
        while ((rp_timer_now_us() - tick_start) < TICK_US) { }
    }
}
