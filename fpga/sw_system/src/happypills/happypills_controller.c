
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
#include <lcd.h>
#include "happypills_hmi.h"
#include "happypills_controller.h"

static void controller_task(uint32_t elapsed);

static controller_state_t controller_state = CONTROLLER_STATE_UNDEFINIED;
static scheduler_entry_t happypills_controller_entry = {0, 100, controller_task};

void happypills_controller_init(void) {
    actuators_init();
    irproximity_init();

    scheduler_add_entry(&happypills_controller_entry);

    LOG_OK();
}

static void controller_task(uint32_t elapsed) {
    rtc_data_t time, alarm1, alarm2;
    rtc_get_now(&time);
    rtc_get_alarm_1(&alarm1);
    rtc_get_alarm_2(&alarm2);

    switch (controller_state) {
        case CONTROLLER_STATE_IDLE:
            if (lcd_get_cursor() == LCD_CURSOR_NONE && (hmi_next_pill() == TRUE ||
                                                        rtc_is_equal(&time, &alarm1) == TRUE ||
                                                        rtc_is_equal(&time, &alarm2) == TRUE)) {
                actuators_set_motor(DINOUTS_HIGH);

                controller_state = CONTROLLER_STATE_ROTATING;
                LOG("[%s] Next pill trigger received, rotating", rtc_get_timestamp_str(&time));
            }
            break;
        case CONTROLLER_STATE_ROTATING:
            if (irproximity_read() == DINOUTS_HIGH) {
                actuators_set_motor(DINOUTS_LOW);
                actuators_buzzer_turn_on();

                controller_state = CONTROLLER_STATE_WAITING;
                LOG("[%s] Pill detected, waiting for removal", rtc_get_timestamp_str(&time));
            }
            break;
        case CONTROLLER_STATE_WAITING:
            if (irproximity_read() == DINOUTS_LOW) {
                actuators_buzzer_turn_off();

                controller_state = CONTROLLER_STATE_SYNCH;
                LOG("[%s] Pills removed, going to synch.", rtc_get_timestamp_str(&time));
            } else if (hmi_next_pill() == TRUE) {
                actuators_buzzer_turn_off();
                actuators_set_motor(DINOUTS_HIGH);

                controller_state = CONTROLLER_STATE_ROTATING;
                LOG("[%s] Pills skip received", rtc_get_timestamp_str(&time));
            }
            break;
        case CONTROLLER_STATE_SYNCH:
            if (rtc_is_equal(&time, &alarm1) == FALSE &&
                rtc_is_equal(&time, &alarm2) == FALSE) {
                controller_state = CONTROLLER_STATE_IDLE;
                LOG("[%s] Pills removed, going to idle.", rtc_get_timestamp_str(&time));
            }
            break;
        default:
            controller_state = CONTROLLER_STATE_IDLE;
    }
}

controller_state_t controller_get_state(void) {
    return controller_state;
}