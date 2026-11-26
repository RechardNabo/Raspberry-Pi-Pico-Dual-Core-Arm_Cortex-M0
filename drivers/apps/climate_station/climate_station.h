/* climate_station.h — integration app 1: triple-sensor climate monitor.
 * Devices: DS18B20 (#19 precision digital) + LM35 (#21 analog) + DHT11
 * (#4 humidity) + RGB LED (#12 status). The ADVANCED part: cross-validation
 * — two independent temperature technologies must agree within 3C or the
 * app reports SENSOR_FAULT instead of trusting either one. RGB maps the
 * temperature band: blue cold, green comfortable, red hot, violet fault.
 * Non-blocking FSM — one step per call, no waits inside. */
#ifndef CLIMATE_STATION_H
#define CLIMATE_STATION_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    CLIMATE_STATE_INIT = 0,
    CLIMATE_STATE_READ_LM35,
    CLIMATE_STATE_READ_DS18B20,
    CLIMATE_STATE_READ_DHT11,
    CLIMATE_STATE_EVALUATE,
    CLIMATE_STATE_SENSOR_FAULT
} climate_state_t;

typedef enum {
    CLIMATE_BAND_COLD = 0,     /* < 18C: blue */
    CLIMATE_BAND_COMFORT,      /* 18-26C: green */
    CLIMATE_BAND_HOT,          /* > 26C: red */
    CLIMATE_BAND_FAULT         /* disagreement: violet */
} climate_band_t;

typedef struct {
    climate_state_t state;
    int32_t lm35_centic;
    int32_t ds18b20_centic;
    uint8_t humidity;
    climate_band_t band;
    uint32_t fault_count;
} climate_station_t;

void climate_station_init(climate_station_t *app);
void climate_station_step(climate_station_t *app);   /* call every loop pass */
climate_band_t climate_classify(int32_t centic);
bool climate_sensors_agree(int32_t a_centic, int32_t b_centic);

#endif
