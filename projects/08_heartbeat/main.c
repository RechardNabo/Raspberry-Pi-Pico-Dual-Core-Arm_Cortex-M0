/* main.c — kit module #8 Finger Heartbeat PPG — standalone firmware, RP2040.
 * Non-blocking super-loop over the device layer. Wiring/verification: README.md.
 * Target build: Pico SDK. Host-verified logic: drivers/tests/unit/. */
#include <stdint.h>
#include <stdbool.h>

#include "../../drivers/devices/kit/kit.h"
#include "../../drivers/peripherals/gpio/rp_gpio.h"
#include "../../drivers/peripherals/timer/rp_timer.h"

#define TICK_US  100000UL
#define LED_PIN  25U   /* Pico onboard LED — the universal demo indicator */

int main(void)
{
    rp_timer_init();
    analog_in_t sensor;
    analog_in_config_t cfg = kit_analog_config(KIT_HEARTBEAT, 0U);
    analog_in_init(&sensor, &cfg);
    rp_adc_init();
    int32_t eng = 0;

    for (;;) {
        const uint64_t tick_start = rp_timer_now_us();
        if (analog_in_read_scaled(&sensor, &eng) == ANALOG_IN_STATUS_OK) {
            /* report eng over UART console — scaling per kit table */
        }
        while ((rp_timer_now_us() - tick_start) < TICK_US) { }
    }
}
