
/* Standard C Includes */
#include <stdio.h>
#include <inttypes.h>

/* BSP & Xilinx Includes */

/* Project includes */
#include <platform.h>
#include <dinouts.h>
#include <buttons_cfg.h>
#include <scheduler.h>
#include "buttons.h"

#ifdef PLATFORM_BUTTONS_MONITOR_PERIOD

static void buttons_monitor_task (uint32_t elapsed);

static scheduler_entry_t buttons_monitor_entry = {0, PLATFORM_BUTTONS_MONITOR_PERIOD, buttons_monitor_task};

static void buttons_monitor_task (uint32_t elapsed) {
    dinouts_level_t b1, b2, b3;
    dinouts_turn_on(PLATFORM_BUTTONS_PIN_POWER_SUPPLY);
    b1 = dinouts_read(PLATFORM_BUTTONS_PIN_BUTTON_1);
    b2 = dinouts_read(PLATFORM_BUTTONS_PIN_BUTTON_2);
    b3 = dinouts_read(PLATFORM_BUTTONS_PIN_BUTTON_3);
    dinouts_turn_off(PLATFORM_BUTTONS_PIN_POWER_SUPPLY);
    LOG("Button 1: %4s; Button 2: %4s; Button 3: %4s;", DINOUT_LEVEL_STR(b1), DINOUT_LEVEL_STR(b2), DINOUT_LEVEL_STR(b3));
}

#endif /* PLATFORM_BUTTONS_MONITOR_PERIOD */

void buttons_init (void) {
    dinouts_init();

    dinouts_set_direction(PLATFORM_BUTTONS_PIN_POWER_SUPPLY, DINOUTS_DIRECTION_OUT);
    dinouts_set_direction(PLATFORM_BUTTONS_PIN_BUTTON_1, DINOUTS_DIRECTION_IN);
    dinouts_set_direction(PLATFORM_BUTTONS_PIN_BUTTON_2, DINOUTS_DIRECTION_IN);
    dinouts_set_direction(PLATFORM_BUTTONS_PIN_BUTTON_3, DINOUTS_DIRECTION_IN);

#ifdef PLATFORM_BUTTONS_MONITOR_PERIOD
    scheduler_add_entry(&buttons_monitor_entry);
#endif /* PLATFORM_BUTTONS_MONITOR_PERIOD */

    LOG_OK();
}

dinouts_level_t buttons_read (buttons_t button) {
    dinouts_level_t value;

    dinouts_turn_on(PLATFORM_BUTTONS_PIN_POWER_SUPPLY);
    value = dinouts_read((dinouts_pin_t) button);
    dinouts_turn_off(PLATFORM_BUTTONS_PIN_POWER_SUPPLY);

    return value;
}