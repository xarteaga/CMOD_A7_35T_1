
/* Standard C Includes */
#include <stdio.h>
#include <inttypes.h>

/* BSP & Xilinx Includes */

/* Project includes */
#include <platform.h>
#include <dinouts.h>
#include <actuators_cfg.h>
#include <scheduler.h>
#include "actuators.h"

static dinouts_level_t actuators_motor_status = DINOUTS_UNDEFINED;
static dinouts_level_t actuators_buzzer_status = DINOUTS_UNDEFINED;

#ifdef PLATFORM_ACTUATORS_MONITOR_PERIOD

static void actuators_monitor_task (uint32_t elapsed);

static scheduler_entry_t actuators_monitor_entry = {0, PLATFORM_ACTUATORS_MONITOR_PERIOD, actuators_monitor_task};

static void actuators_monitor_task (uint32_t elapsed) {
    LOG("Motor status: %s", DINOUT_LEVEL_STR(actuators_motor_status));
}

#endif /* PLATFORM_actuators_MONITOR_PERIOD */

void actuators_init(void) {
    dinouts_init();

    dinouts_set_direction(PLATFORM_ACTUATORS_PIN_MOTOR, DINOUTS_DIRECTION_OUT);
    dinouts_turn_off(PLATFORM_ACTUATORS_PIN_MOTOR);
    dinouts_turn_off(PLATFORM_ACTUATORS_PIN_BUZZER);

#ifdef PLATFORM_ACTUATORS_MONITOR_PERIOD
    scheduler_add_entry(&actuators_monitor_entry);
#endif /* PLATFORM_ACTUATORS_MONITOR_PERIOD */

    LOG_OK();
}

void actuators_set_motor(dinouts_level_t level) {
    if (level == DINOUTS_HIGH) {
        dinouts_turn_on(PLATFORM_ACTUATORS_PIN_MOTOR);
        actuators_motor_status = DINOUTS_HIGH;
    } else {
        dinouts_turn_off(PLATFORM_ACTUATORS_PIN_MOTOR);
        actuators_motor_status = DINOUTS_LOW;
    }
}

void actuators_buzzer_turn_on(void) {
    dinouts_turn_on(PLATFORM_ACTUATORS_PIN_BUZZER);
    actuators_buzzer_status = DINOUTS_HIGH;
}

void actuators_buzzer_turn_off(void) {
    dinouts_turn_off(PLATFORM_ACTUATORS_PIN_BUZZER);
    actuators_buzzer_status = DINOUTS_LOW;
}