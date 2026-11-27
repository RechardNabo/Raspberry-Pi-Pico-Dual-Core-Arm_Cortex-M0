/* knock_lock_app.h — secret-knock access control.
 * Integration: knock sensor edges within a time window form a pattern;
 * exactly KL_REQUIRED knocks inside KL_WINDOW_TICKS unlocks the relay
 * for KL_UNLOCK_TICKS, then it relocks automatically.
 * The fusion contract: sparse knocks never accumulate — the window
 * resets the count, and a partial pattern decays to nothing. */
#ifndef KNOCK_LOCK_APP_H
#define KNOCK_LOCK_APP_H

#include <stdint.h>
#include <stdbool.h>
#include "../../../drivers/devices/dig_in/dig_in.h"
#include "../../../drivers/devices/dig_out/dig_out.h"
#include "../../lib/edge_count.h"

#define KL_REQUIRED       3U
#define KL_WINDOW_TICKS   1000U
#define KL_UNLOCK_TICKS   500U

typedef struct {
    dig_in_t     knock;
    dig_out_t    relay;
    dig_out_t    led;
    edge_count_t edges;
    uint32_t     count;
    uint32_t     window;
    uint32_t     unlock_timer;
    bool         unlocked;
} knock_lock_app_t;

void knock_lock_app_init(knock_lock_app_t *app, uint8_t knock_pin,
                         uint8_t relay_pin, uint8_t led_pin);
void knock_lock_app_step(knock_lock_app_t *app);
bool knock_lock_unlocked(const knock_lock_app_t *app);

#endif
