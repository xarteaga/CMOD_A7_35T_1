
/* Standard C Includes */
#include <stdio.h>
#include <inttypes.h>

/* BSP & Xilinx Includes */
#include <xgpio.h>

/* Project includes */
#include <platform.h>
#include <scheduler.h>
#include <dinouts.h>
#include <actuators.h>
#include <irproximity.h>
#include <buttons.h>
#include <rtc.h>
#include "happypills_hmi.h"

typedef enum {
    HAPPYPILLS_STATE_UNDEFINIED = 0,
    HAPPYPILLS_STATE_ROTATING = 1,
    HAPPYPILLS_STATE_WAITING = 2,
    HAPPYPILLS_STATE_IDLE = 3,
} happypills_state_t;

static void happypills_controller_task(uint32_t elapsed);

static happypills_state_t happypills_state = HAPPYPILLS_STATE_UNDEFINIED;
static scheduler_entry_t happypills_controller_entry = {0, 100, happypills_controller_task};

void happypills_controller_init(void) {
    actuators_init();
    irproximity_init();

    scheduler_add_entry(&happypills_controller_entry);

    LOG_OK();
}

static void happypills_controller_task(uint32_t elapsed) {
    rtc_data_t *time = rtc_get_now();

    switch (happypills_state) {
        case HAPPYPILLS_STATE_IDLE:
            if (hmi_next_pill() == TRUE) {
                actuators_set_motor(DINOUTS_HIGH);

                happypills_state = HAPPYPILLS_STATE_ROTATING;
                LOG("[%s] Next pill trigger received, rotating", rtc_get_timestamp_str(&time));
            }
            break;
        case HAPPYPILLS_STATE_ROTATING:
            if (irproximity_read() == DINOUTS_HIGH) {
                actuators_set_motor(DINOUTS_LOW);
                actuators_buzzer_turn_on();

                happypills_state = HAPPYPILLS_STATE_WAITING;
                LOG("[%s] Pill detected, waiting for removal", rtc_get_timestamp_str(&time));
            }
            break;
        case HAPPYPILLS_STATE_WAITING:
            if (irproximity_read() == DINOUTS_LOW) {
                actuators_buzzer_turn_off();

                happypills_state = HAPPYPILLS_STATE_IDLE;
                LOG("[%s] Pills removed, going to idle", rtc_get_timestamp_str(&time));
            } else if (buttons_read(BUTTONS_1) == DINOUTS_HIGH) {
                actuators_buzzer_turn_off();
                actuators_set_motor(DINOUTS_HIGH);

                happypills_state = HAPPYPILLS_STATE_ROTATING;
                LOG("[%s] Pills skip received", rtc_get_timestamp_str(&time));
            }
            break;
        default:
            happypills_state = HAPPYPILLS_STATE_IDLE;
    }
}