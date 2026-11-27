#include "knock_lock_app.h"
#include "../../../drivers/devices/kit/kit.h"

void knock_lock_app_init(knock_lock_app_t *app, uint8_t knock_pin,
                         uint8_t relay_pin, uint8_t led_pin)
{
    dig_in_config_t  kcfg = kit_dig_in_config(KIT_KNOCK, knock_pin);
    dig_out_config_t rcfg = kit_dig_out_config(KIT_RELAY, relay_pin);
    dig_out_config_t lcfg = kit_dig_out_config(KIT_DUAL_LED, led_pin);
    dig_in_init(&app->knock, &kcfg);
    dig_out_init(&app->relay, &rcfg);
    dig_out_init(&app->led, &lcfg);
    edge_count_init(&app->edges);
    app->count = 0U;
    app->window = 0U;
    app->unlock_timer = 0U;
    app->unlocked = false;
    dig_out_set(&app->relay, false);
    dig_out_set(&app->led, false);
}

void knock_lock_app_step(knock_lock_app_t *app)
{
    dig_in_sample(&app->knock);
    bool edge = edge_count_step(&app->edges, dig_in_state(&app->knock));

    if (!app->unlocked) {
        if (edge) {
            if (app->count == 0U) {
                app->window = 0U;   /* first knock opens the window */
            }
            app->count++;
            if (app->count >= KL_REQUIRED) {
                app->unlocked = true;
                app->unlock_timer = 0U;
                app->count = 0U;
            }
        }

        /* window expiry: partial patterns decay to nothing */
        if (app->count > 0U) {
            app->window++;
            if (app->window >= KL_WINDOW_TICKS) {
                app->count = 0U;
            }
        }
    } else {
        app->unlock_timer++;
        if (app->unlock_timer >= KL_UNLOCK_TICKS) {
            app->unlocked = false;
        }
    }

    dig_out_set(&app->relay, app->unlocked);
    dig_out_set(&app->led, app->unlocked);
}

bool knock_lock_unlocked(const knock_lock_app_t *app)
{
    return app->unlocked;
}
