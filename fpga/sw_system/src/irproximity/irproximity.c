
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
#include "irproximity.h"

#ifdef PLATFORM_IRPROXIMITY_MONITOR_PERIOD

static void irproximity_monitor_task(uint32_t elapsed);

static scheduler_entry_t irproximity_monitor_entry = {0, PLATFORM_IRPROXIMITY_MONITOR_PERIOD, irproximity_monitor_task};

static void irproximity_monitor_task(uint32_t elapsed) {
    uint16_t mv;
    dinouts_level_t p = DINOUTS_UNDEFINED;

    dinouts_turn_on(PLATFORM_IRPROXIMITY_PIN_POWER_SUPPLY);
    sleep(0.01);
    mv = xadc_read_mv(PLATFORM_IRPROXIMITY_ADC);
#ifndef PLATFORM_IRPROXIMITY_TURN_OFF
    dinouts_turn_off(PLATFORM_IRPROXIMITY_PIN_POWER_SUPPLY);
#endif /* PLATFORM_IRPROXIMITY_TURN_OFF */

    if (mv > PLATFORM_IRPROXIMITY_THREASHOLD) {
        p = DINOUTS_HIGH;
    } else {
        p = DINOUTS_LOW;
    }

    LOG("Voltage %4d mV; Decission: %s", mv, DINOUT_LEVEL_STR(p));
}

#endif /* PLATFORM_irproximity_MONITOR_PERIOD */

void irproximity_init(void) {
    dinouts_init();
    xadc_init();

    dinouts_set_direction(PLATFORM_IRPROXIMITY_PIN_POWER_SUPPLY, DINOUTS_DIRECTION_OUT);
    dinouts_turn_off(PLATFORM_IRPROXIMITY_PIN_POWER_SUPPLY);

#ifdef PLATFORM_IRPROXIMITY_MONITOR_PERIOD
    scheduler_add_entry(&irproximity_monitor_entry);
#endif /* PLATFORM_IRPROXIMITY_MONITOR_PERIOD */

    LOG_OK();
}

dinouts_pin_t irproximity_read(void) {
    uint16_t mv;

    dinouts_turn_on(PLATFORM_IRPROXIMITY_PIN_POWER_SUPPLY);

    sleep(0.01);
    mv = xadc_read_mv(PLATFORM_IRPROXIMITY_ADC);
#ifndef PLATFORM_IRPROXIMITY_TURN_OFF
    dinouts_turn_off(PLATFORM_IRPROXIMITY_PIN_POWER_SUPPLY);
#endif /* PLATFORM_IRPROXIMITY_TURN_OFF */

    if (mv > PLATFORM_IRPROXIMITY_THREASHOLD) {
        return DINOUTS_HIGH;
    }
    return DINOUTS_LOW;
}