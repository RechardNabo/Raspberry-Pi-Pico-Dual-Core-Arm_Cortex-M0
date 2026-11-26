/* main.c — kit module #19 DS18B20 Precision Temp — standalone firmware, RP2040.
 * Non-blocking super-loop over the device layer. Wiring/verification: README.md.
 * Target build: Pico SDK. Host-verified logic: drivers/tests/unit/. */
#include <stdint.h>
#include <stdbool.h>

#include "../../drivers/devices/kit/kit.h"
#include "../../drivers/peripherals/gpio/rp_gpio.h"
#include "../../drivers/peripherals/timer/rp_timer.h"

#define TICK_US  1000000UL
#define LED_PIN  25U   /* Pico onboard LED — the universal demo indicator */

int main(void)
{
    rp_timer_init();
    ds18b20_t ds;
    ds18b20_init(&ds, 22U);
    uint8_t scratch[9];

    for (;;) {
        const uint64_t tick_start = rp_timer_now_us();
        if (ds18b20_start_conversion(&ds) == ONEWIRE_STATUS_OK) {
            /* 750ms conversion time on hardware — the non-blocking design
             * reads last conversion then starts the next; host model is
             * instant, target sequences via the FSM tick */
        }
        if (ds18b20_read_scratchpad(&ds, scratch) == ONEWIRE_STATUS_OK) {
            const int32_t millic = ds18b20_raw_to_millic((int16_t)(scratch[0] | ((uint16_t)scratch[1] << 8)));
            /* report millic over UART console */
        }
        while ((rp_timer_now_us() - tick_start) < TICK_US) { }
    }
}
