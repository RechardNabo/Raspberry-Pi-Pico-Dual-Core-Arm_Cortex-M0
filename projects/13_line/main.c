/* main.c — kit module #13 Line Sensor TCRT5000 — standalone firmware, RP2040.
 * Non-blocking super-loop over the device layer. Wiring/verification: README.md.
 * Target build: Pico SDK. Host-verified logic: drivers/tests/unit/. */
#include <stdint.h>
#include <stdbool.h>

#include "../../drivers/devices/kit/kit.h"
#include "../../drivers/peripherals/gpio/rp_gpio.h"
#include "../../drivers/peripherals/timer/rp_timer.h"

#define TICK_US  10000UL
#define LED_PIN  25U   /* Pico onboard LED — the universal demo indicator */

int main(void)
{
    rp_timer_init();
    dig_in_t sensor;
    dig_in_config_t cfg = kit_dig_in_config(KIT_LINE, 13U);
    dig_in_init(&sensor, &cfg);
    rp_gpio_set_output(LED_PIN);
    uint32_t events = 0U;
    bool was_active = false;

    for (;;) {
        const uint64_t tick_start = rp_timer_now_us();
        dig_in_sample(&sensor);
        const bool active = dig_in_state(&sensor);
        rp_gpio_write(LED_PIN, active);
        if (active && !was_active) {
            events++;   /* edge count — report over UART console */
        }
        was_active = active;
        while ((rp_timer_now_us() - tick_start) < TICK_US) { }
    }
}
