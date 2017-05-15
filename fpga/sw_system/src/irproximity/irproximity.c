
/* Standard C Includes */
#include <stdio.h>
#include <inttypes.h>

/* BSP & Xilinx Includes */

/* Project includes */
#include <platform.h>
#include <dinouts.h>
#include <xadc.h>
#include <irproximity_cfg.h>
#include <scheduler.h>
#include <xenv.h>
#include "irproximity.h"

static void irproximity_cont_task(uint32_t elapsed);

static uint16_t irproximity_monitor_index = 0;
static uint16_t irproximity_monitor_average_env = 0;
static uint16_t irproximity_monitor_average = 0;
static int16_t irproximity_monitor_diff = 0;
static uint16_t irproximity_monitor_buffer_env[PLATFORM_IRPROXIMITY_AVERAGING] = {0};
static uint16_t irproximity_monitor_buffer[PLATFORM_IRPROXIMITY_AVERAGING] = {0};
static scheduler_entry_t irproximity_cont_entry = {0, 0, irproximity_cont_task};


#ifdef PLATFORM_IRPROXIMITY_MONITOR_PERIOD

static void irproximity_monitor_task(uint32_t elapsed);

static scheduler_entry_t irproximity_monitor_entry = {0, 0, irproximity_monitor_task};

static void irproximity_monitor_task(uint32_t elapsed) {
    LOG("Average %4d mV; Decision: %s; Environment: %4d mV; Diff: %4d mV", (int) irproximity_monitor_average,
        DINOUT_LEVEL_STR(irproximity_read()), (int) irproximity_monitor_average_env, (int) irproximity_monitor_diff);
}

#endif /* PLATFORM_irproximity_MONITOR_PERIOD */

void irproximity_init(void) {
    dinouts_init();
    xadc_init();

    dinouts_set_direction(PLATFORM_IRPROXIMITY_ENABLE_TX, DINOUTS_DIRECTION_OUT);
    dinouts_set_direction(PLATFORM_IRPROXIMITY_ENABLE_RX, DINOUTS_DIRECTION_OUT);
    dinouts_turn_off(PLATFORM_IRPROXIMITY_ENABLE_TX);
    dinouts_turn_off(PLATFORM_IRPROXIMITY_ENABLE_RX);

    scheduler_add_entry(&irproximity_cont_entry);

#ifdef PLATFORM_IRPROXIMITY_MONITOR_PERIOD
    scheduler_add_entry(&irproximity_monitor_entry);
#endif /* PLATFORM_IRPROXIMITY_MONITOR_PERIOD */

    irproximity_start();

    LOG_OK();
}

dinouts_level_t irproximity_read(void) {
    if (irproximity_monitor_diff > PLATFORM_IRPROXIMITY_THREASHOLD) {
        return DINOUTS_HIGH;
    }
    return DINOUTS_LOW;
}

static void irproximity_cont_task(uint32_t elapsed) {
    int i = 0;
    uint16_t temp_average = 0, temp_average_env = 0;

    dinouts_turn_on(PLATFORM_IRPROXIMITY_ENABLE_TX);
    udelay(1000);

    irproximity_monitor_buffer[irproximity_monitor_index] = xadc_read_mv(PLATFORM_IRPROXIMITY_ADC);
    for (i = 0; i < PLATFORM_IRPROXIMITY_AVERAGING; i++) {
        temp_average += irproximity_monitor_buffer[i];
    }
    irproximity_monitor_average = (uint16_t) (temp_average / PLATFORM_IRPROXIMITY_AVERAGING);

    dinouts_turn_off(PLATFORM_IRPROXIMITY_ENABLE_TX);
    udelay(1000);

    irproximity_monitor_buffer_env[irproximity_monitor_index] = xadc_read_mv(PLATFORM_IRPROXIMITY_ADC);
    for (i = 0; i < PLATFORM_IRPROXIMITY_AVERAGING; i++) {
        temp_average_env += irproximity_monitor_buffer_env[i];
    }
    irproximity_monitor_average_env = (uint16_t) (temp_average_env / PLATFORM_IRPROXIMITY_AVERAGING);

    if (irproximity_monitor_average_env > irproximity_monitor_average) {
        irproximity_monitor_diff = (int16_t) irproximity_monitor_average_env - (int16_t) irproximity_monitor_average;
    } else {
        irproximity_monitor_diff = (int16_t) irproximity_monitor_average - (int16_t) irproximity_monitor_average_env;
    }


    irproximity_monitor_index = (uint8_t) ((irproximity_monitor_index + 1) % PLATFORM_IRPROXIMITY_AVERAGING);
}

void irproximity_start(void) {
    dinouts_turn_on(PLATFORM_IRPROXIMITY_ENABLE_RX);
    irproximity_monitor_index = 0;
    irproximity_monitor_average = 0;
    memset(irproximity_monitor_buffer, 0, sizeof(irproximity_monitor_buffer));
    irproximity_cont_entry.period = PLATFORM_IRPROXIMITY_UPDATE_PERIOD;
#ifdef PLATFORM_IRPROXIMITY_MONITOR_PERIOD
    irproximity_monitor_entry.period = PLATFORM_IRPROXIMITY_MONITOR_PERIOD;
#endif /* PLATFORM_IRPROXIMITY_MONITOR_PERIOD */
}

void irproximity_stop(void) {
    irproximity_cont_entry.period = 0;
#ifdef PLATFORM_IRPROXIMITY_MONITOR_PERIOD
    irproximity_monitor_entry.period = 0;
#endif /* PLATFORM_IRPROXIMITY_MONITOR_PERIOD */
    dinouts_turn_off(PLATFORM_IRPROXIMITY_ENABLE_RX);
}