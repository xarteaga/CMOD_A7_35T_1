
/* Standard C Includes */
#include <stdio.h>
#include <inttypes.h>

/* BSP & Xilinx Includes */
#include <xgpio.h>

/* Project includes */
#include <platform.h>
#include <scheduler.h>
#include <rtc.h>
#include <lcd.h>
#include "happypills_controller.h"
#include "happypills_hmi.h"

static void happypills_task(uint32_t elapsed);

static scheduler_entry_t happypills_entry = {0, 50, happypills_task};

void happypills_init(void) {
    happypills_controller_init();
    happypills_hmi_init();

    scheduler_add_entry(&happypills_entry);

    LOG_OK();
}

static void happypills_task(uint32_t elapsed) {
    rtc_data_t temp = {0};
    hmi_state_t hmi_state = hmi_get_state();
    controller_state_t controller_state = controller_get_state();

    /* Update LCD data */
    rtc_get_now(&temp);
    lcd_set_time(&temp);

    rtc_get_alarm_1(&temp);
    lcd_set_alarm1(&temp);

    rtc_get_alarm_2(&temp);
    lcd_set_alarm2(&temp);

    if (hmi_state == HMI_STATE_ON || controller_state == CONTROLLER_STATE_WAITING) {
        lcd_set_backlight(0x1D);
    } else {
        lcd_set_backlight(0x00);
    }
}
