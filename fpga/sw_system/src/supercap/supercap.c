//
// Created by vaxi on 5/2/17.
//

/* Standard C Includes */
#include <stdio.h>

/* BSP & Xilinx Includes */
#include <xstatus.h>

/* Project includes */
#include <platform.h>
#include <scheduler.h>
#include <xadc.h>
#include "supercap.h"
#include "supercap_cfg.h"
#include "../leds/leds.h"

static void supercap_update(uint32_t elapsed);

static uint8_t supercap_percent = 0;
static uint16_t supercap_charge = 0;
static uint16_t supercap_measure = 0;
static scheduler_entry_t supercap_update_entry = {0, PLATFORM_SUPERCAP_UPDATE_PERIOD, supercap_update};

#ifdef PLATFORM_SUPERCAP_MONITOR_PERIOD

void supercap_monitor(uint32_t elapsed);

static scheduler_entry_t supercap_monitor_entry = {0, PLATFORM_SUPERCAP_MONITOR_PERIOD, supercap_monitor};

void supercap_monitor(uint32_t elapsed) {
    LOG("Percent: %3d %%; Charge: %4d mV; Measure: %4d mV", (int) supercap_percent, (int) supercap_charge,
        (int) supercap_measure);
}

#endif /* PLATFORM_SUPERCAP_MONITOR_PERIOD */

int supercap_init(void) {

    int ret ;

    ret = xadc_init();
    if (ret != XST_SUCCESS) {
        LOG_ERROR();
    }

    ret = leds_init();
    if (ret != XST_SUCCESS) {
        LOG_ERROR();
    }

    scheduler_add_entry(&supercap_update_entry);
#ifdef PLATFORM_SUPERCAP_MONITOR_PERIOD
    scheduler_add_entry(&supercap_monitor_entry);

#endif /* PLATFORM_SUPERCAP_MONITOR_PERIOD */

    LOG_OK();
    return ret;
}

static void supercap_update(uint32_t elapsed) {
    supercap_measure = xadc_read_mv(PLATFORM_SUPERCAP_INPUT);
    supercap_charge = supercap_measure * (int16_t)PLATFORM_SUPERCAP_VOLTAGE_SCALE;
    supercap_percent = (uint8_t) ((100 * supercap_charge) / PLATFORM_SUPERCAP_VOLTAGE_MAX);

    if (supercap_percent > PLATFORM_SUPERCAP_THREASHOLD_GREEN) {
        leds_turn_on(LEDS_CHAN_GREEN);
        leds_turn_off(LEDS_CHAN_RED);
    } else {
        leds_turn_off(LEDS_CHAN_GREEN);
        leds_turn_on(LEDS_CHAN_RED);
    }
}