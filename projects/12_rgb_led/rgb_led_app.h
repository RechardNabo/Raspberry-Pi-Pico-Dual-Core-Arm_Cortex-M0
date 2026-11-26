/* rgb_led_app.h — continuous rainbow rotation.
 * Composition: colorwheel pure function + rgb_led PWM device. */
#ifndef RGB_LED_APP_H
#define RGB_LED_APP_H

#include <stdint.h>
#include <stdbool.h>
#include "../../drivers/devices/pwm_out/pwm_out.h"
#include "../lib/colorwheel.h"

typedef struct {
    rgb_led_t device;
    uint16_t hue;
} rgb_led_app_t;

void rgb_led_app_init(rgb_led_app_t *app, uint8_t rs, uint8_t rc, uint8_t gs, uint8_t gc,
                      uint8_t bs, uint8_t bc, uint32_t clk_hz, uint32_t freq);
void rgb_led_app_step(rgb_led_app_t *app);   /* one call per tick — hue advances 1 */
uint16_t rgb_led_app_hue(const rgb_led_app_t *app);

#endif
