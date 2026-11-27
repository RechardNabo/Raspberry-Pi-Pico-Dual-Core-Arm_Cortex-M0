/* security_alarm_app.h — armed intrusion alarm finite state machine.
 * Integration: vibration sensor (trigger) + button (arm/disarm) drive
 * buzzer + LED through an arming state machine:
 *   DISARMED → (short press) → ARMING → (exit delay) → ARMED
 *   ARMED + vibration edge → ALARM (latched)
 *   ALARM/ARMED + long hold → DISARMED
 * The fusion contract: vibration means nothing unless armed; the alarm
 * latch survives the trigger stopping; only a deliberate long hold disarms. */
#ifndef SECURITY_ALARM_APP_H
#define SECURITY_ALARM_APP_H

#include <stdint.h>
#include <stdbool.h>
#include "../../../drivers/devices/dig_in/dig_in.h"
#include "../../../drivers/devices/dig_out/dig_out.h"
#include "../../lib/edge_count.h"

#define SA_EXIT_DELAY_TICKS   500U    /* arming grace period */
#define SA_DISARM_HOLD_TICKS  100U    /* long-hold to disarm */

typedef enum {
    SA_DISARMED = 0,
    SA_ARMING,
    SA_ARMED,
    SA_ALARM
} sa_state_t;

typedef struct {
    dig_in_t    vibration;
    dig_in_t    button;
    dig_out_t   buzzer;
    dig_out_t   led;
    edge_count_t vib_edges;
    sa_state_t  state;
    uint32_t    timer;
    uint32_t    hold;
} security_alarm_app_t;

void security_alarm_app_init(security_alarm_app_t *app, uint8_t vib_pin, uint8_t btn_pin,
                             uint8_t buzzer_pin, uint8_t led_pin);
void security_alarm_app_step(security_alarm_app_t *app);
sa_state_t security_alarm_state(const security_alarm_app_t *app);
bool security_alarm_sounding(const security_alarm_app_t *app);

#endif
