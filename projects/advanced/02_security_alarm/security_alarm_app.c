#include "security_alarm_app.h"
#include "../../../drivers/devices/kit/kit.h"

void security_alarm_app_init(security_alarm_app_t *app, uint8_t vib_pin, uint8_t btn_pin,
                             uint8_t buzzer_pin, uint8_t led_pin)
{
    dig_in_config_t  vcfg = kit_dig_in_config(KIT_VIBRATION, vib_pin);
    dig_in_config_t  bcfg = kit_dig_in_config(KIT_BUTTON, btn_pin);
    dig_out_config_t zcfg = kit_dig_out_config(KIT_BUZZER_ACTIVE, buzzer_pin);
    dig_out_config_t lcfg = kit_dig_out_config(KIT_DUAL_LED, led_pin);
    dig_in_init(&app->vibration, &vcfg);
    dig_in_init(&app->button, &bcfg);
    dig_out_init(&app->buzzer, &zcfg);
    dig_out_init(&app->led, &lcfg);
    edge_count_init(&app->vib_edges);
    app->state = SA_DISARMED;
    app->timer = 0U;
    app->hold = 0U;
    dig_out_set(&app->buzzer, false);
    dig_out_set(&app->led, false);
}

void security_alarm_app_step(security_alarm_app_t *app)
{
    dig_in_sample(&app->vibration);
    dig_in_sample(&app->button);
    bool vib = dig_in_state(&app->vibration);
    bool btn = dig_in_state(&app->button);
    bool vib_edge = edge_count_step(&app->vib_edges, vib);

    if (btn) {
        if (app->hold < 0xFFFFFFFFU) app->hold++;
    } else {
        /* released: a short press arms when disarmed */
        if (app->hold > 0U && app->hold < SA_DISARM_HOLD_TICKS && app->state == SA_DISARMED) {
            app->state = SA_ARMING;
            app->timer = 0U;
        }
        app->hold = 0U;
    }

    switch (app->state) {
    case SA_DISARMED:
        dig_out_set(&app->buzzer, false);
        dig_out_set(&app->led, false);
        break;
    case SA_ARMING:
        dig_out_set(&app->led, true);           /* arming indicator */
        dig_out_set(&app->buzzer, false);
        app->timer++;
        if (app->timer >= SA_EXIT_DELAY_TICKS) {
            app->state = SA_ARMED;
        }
        break;
    case SA_ARMED:
        dig_out_set(&app->led, false);
        dig_out_set(&app->buzzer, false);
        if (vib_edge) {
            app->state = SA_ALARM;
        }
        break;
    case SA_ALARM:
        dig_out_set(&app->led, true);
        dig_out_set(&app->buzzer, true);
        break;
    }

    /* long hold disarms from any armed state */
    if (app->hold >= SA_DISARM_HOLD_TICKS && app->state != SA_DISARMED) {
        app->state = SA_DISARMED;
        app->hold = 0U;
        dig_out_set(&app->buzzer, false);
        dig_out_set(&app->led, false);
    }
}

sa_state_t security_alarm_state(const security_alarm_app_t *app)
{
    return app->state;
}

bool security_alarm_sounding(const security_alarm_app_t *app)
{
    return app->state == SA_ALARM;
}
