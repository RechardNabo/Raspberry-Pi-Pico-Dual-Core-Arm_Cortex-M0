/* main.c — kit module #4 DHT11 Temp+Humidity — standalone firmware, RP2040.
 * Non-blocking super-loop over the device layer. Wiring/verification: README.md.
 * Target build: Pico SDK. Host-verified logic: drivers/tests/unit/. */
#include <stdint.h>
#include <stdbool.h>

#include "../../drivers/devices/kit/kit.h"
#include "../../drivers/peripherals/gpio/rp_gpio.h"
#include "../../drivers/peripherals/timer/rp_timer.h"

#define TICK_US  2000000UL
#define LED_PIN  25U   /* Pico onboard LED — the universal demo indicator */

int main(void)
{
    rp_timer_init();
    dht11_t dht;
    dht11_init(&dht, 22U);
    uint8_t rh = 0U, temp = 0U;

    for (;;) {
        const uint64_t tick_start = rp_timer_now_us();
        if (dht11_read(&dht, &rh, &temp) == ONEWIRE_STATUS_OK) {
            /* report temp/humidity over UART console */
        }
        while ((rp_timer_now_us() - tick_start) < TICK_US) { }
    }
}
