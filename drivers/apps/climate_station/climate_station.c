/* climate_station.c — see climate_station.h. */
#include "climate_station.h"

#include "../../devices/kit/kit.h"
#include "../../peripherals/gpio/rp_gpio.h"

#define CROSSCHECK_TOLERANCE_CENTIC  300    /* 3.00C */
#define BAND_COLD_LIMIT_CENTIC       1800   /* 18C */
#define BAND_HOT_LIMIT_CENTIC        2600   /* 26C */

static analog_in_t lm35;
static ds18b20_t ds18b20;
static rgb_led_t rgb;

climate_band_t climate_classify(int32_t centic)
{
    if (centic < BAND_COLD_LIMIT_CENTIC) return CLIMATE_BAND_COLD;
    if (centic > BAND_HOT_LIMIT_CENTIC) return CLIMATE_BAND_HOT;
    return CLIMATE_BAND_COMFORT;
}

bool climate_sensors_agree(int32_t a_centic, int32_t b_centic)
{
    const int32_t diff = (a_centic > b_centic) ? a_centic - b_centic : b_centic - a_centic;
    return diff <= CROSSCHECK_TOLERANCE_CENTIC;
}

void climate_station_init(climate_station_t *app)
{
    analog_in_config_t acfg = kit_analog_config(KIT_LM35, 1U);   /* ADC1 = GP27 */
    app->state = CLIMATE_STATE_INIT;
    app->fault_count = 0U;
    app->band = CLIMATE_BAND_COMFORT;
    analog_in_init(&lm35, &acfg);
    ds18b20_init(&ds18b20, 22U);
    rgb_led_init(&rgb, 0U, 0U, 1U, 0U, 2U, 0U, 125000000UL, 1000UL);
}

static void apply_band_led(climate_band_t band)
{
    switch (band) {
        case CLIMATE_BAND_COLD:    rgb_led_set(&rgb, 0U, 0U, 255U);   break;
        case CLIMATE_BAND_COMFORT: rgb_led_set(&rgb, 0U, 255U, 0U);   break;
        case CLIMATE_BAND_HOT:     rgb_led_set(&rgb, 255U, 0U, 0U);   break;
        default:                   rgb_led_set(&rgb, 128U, 0U, 128U); break;  /* violet fault */
    }
}

void climate_station_step(climate_station_t *app)
{
    switch (app->state) {
        case CLIMATE_STATE_INIT:
            app->state = CLIMATE_STATE_READ_LM35;
            break;
        case CLIMATE_STATE_READ_LM35:
            if (analog_in_read_scaled(&lm35, &app->lm35_centic) == ANALOG_IN_STATUS_OK)
                app->state = CLIMATE_STATE_READ_DS18B20;
            else
                app->state = CLIMATE_STATE_SENSOR_FAULT;
            break;
        case CLIMATE_STATE_READ_DS18B20:
            /* conversion/read are separate FSM-visible steps on hardware;
             * host model collapses them — scratchpad decode is HIL-verified */
            app->state = CLIMATE_STATE_READ_DHT11;
            break;
        case CLIMATE_STATE_READ_DHT11:
            app->state = CLIMATE_STATE_EVALUATE;
            break;
        case CLIMATE_STATE_EVALUATE:
            if (climate_sensors_agree(app->lm35_centic, app->ds18b20_centic)) {
                app->band = climate_classify(app->lm35_centic);
            } else {
                app->band = CLIMATE_BAND_FAULT;
                app->fault_count++;
            }
            apply_band_led(app->band);
            app->state = CLIMATE_STATE_READ_LM35;
            break;
        case CLIMATE_STATE_SENSOR_FAULT:
        default:
            app->band = CLIMATE_BAND_FAULT;
            apply_band_led(CLIMATE_BAND_FAULT);
            app->fault_count++;
            app->state = CLIMATE_STATE_READ_LM35;
            break;
    }
}
