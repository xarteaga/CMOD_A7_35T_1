
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
#include "lcd.h"
#include "happypills_hmi.h"


static void hmi_task(uint32_t elapsed);

static scheduler_entry_t hmi_task_entry = {0, 10, hmi_task};
static hmi_state_t hmi_state = HMI_STATE_UNDEFINED;
static uint32_t hmi_timer = 0;
static uint8_t happypills_next = FALSE;

/* Current buttons states */
static dinouts_level_t b1 = DINOUTS_UNDEFINED;
static dinouts_level_t b2 = DINOUTS_UNDEFINED;
static dinouts_level_t b3 = DINOUTS_UNDEFINED;

/* Previous buttons states */
static dinouts_level_t b1p = DINOUTS_UNDEFINED;
static dinouts_level_t b2p = DINOUTS_UNDEFINED;
static dinouts_level_t b3p = DINOUTS_UNDEFINED;

/* Previous title and content */
static uint8_t set_value = 0;

void happypills_hmi_init(void) {
    actuators_init();
    irproximity_init();

    hmi_state = HMI_STATE_ON;
    scheduler_add_entry(&hmi_task_entry);

    /*lcd_set_time(rtc_get_now());
    lcd_set_alarm1(rtc_get_alarm_1());
    lcd_set_alarm2(rtc_get_alarm_2());*/

    LOG_OK();
}

uint8_t hmi_next_pill(void) {
    uint8_t ret = happypills_next;
    happypills_next = FALSE;
    return ret;
}

static void hmi_state_off(void) {
    /* Manage backlight */
    if (b1 == DINOUTS_HIGH || b2 == DINOUTS_HIGH || b3 == DINOUTS_HIGH) {
        lcd_cursor_reset();
        hmi_timer = 0;
        hmi_state = HMI_STATE_ON;
    } else {
        rtc_data_t now = {0};
        rtc_get_now(&now);
        lcd_set_time(&now);
    }
}

static void hmi_state_on(void) {
    rtc_data_t temp;

    if ((b1 == DINOUTS_HIGH && b1 == b1p) ||
        (b2 == DINOUTS_HIGH && b2 == b2p) ||
        (b3 == DINOUTS_HIGH && b3 == b3p)) {
        /* If button has not been release: do nothing */
    } else if (hmi_timer >= 10000) {
        /* Go to OFF if timeout without pushing any button */
        hmi_timer = 0;
        lcd_cursor_reset();
        hmi_state = HMI_STATE_OFF;
        LOG("Buttons timeout %d", (int) lcd_cursor_get());
    } else if (b1 == DINOUTS_HIGH) {
        /* Set button */
        switch (lcd_cursor_get()) {
            case LCD_CURSOR_TIME_HOUR:
                rtc_get_now(&temp);
                set_value = (uint8_t) (temp.hr10 * 10 + temp.hr);
                set_value = (uint8_t)((set_value == 0) ? 23 : set_value - 1);
                temp.hr10 = (uint8_t)(set_value/10);
                temp.hr = (uint8_t)(set_value%10);
                rtc_set_now(&temp);
                break;
            case LCD_CURSOR_TIME_MIN:
                rtc_get_now(&temp);
                set_value = (uint8_t) (temp.min10 * 10 + temp.min);
                set_value = (uint8_t)((set_value == 0) ? 59 : set_value - 1);
                temp.min10 = (uint8_t)(set_value/10);
                temp.min = (uint8_t)(set_value%10);
                rtc_set_now(&temp);
                break;
            case LCD_CURSOR_ALARM_1_HOUR:
                rtc_get_alarm_1(&temp);
                set_value = (uint8_t) (temp.hr10 * 10 + temp.hr);
                set_value = (uint8_t)((set_value == 0) ? 23 : set_value - 1);
                temp.hr10 = (uint8_t)(set_value/10);
                temp.hr = (uint8_t)(set_value%10);
                rtc_set_alarm_1(&temp);
                break;
            case LCD_CURSOR_ALARM_1_MIN:
                rtc_get_alarm_1(&temp);
                set_value = (uint8_t) (temp.min10 * 10 + temp.min);
                set_value = (uint8_t)((set_value == 0) ? 59 : set_value - 1);
                temp.min10 = (uint8_t)(set_value/10);
                temp.min = (uint8_t)(set_value%10);
                rtc_set_alarm_1(&temp);
                break;
            case LCD_CURSOR_ALARM_2_HOUR:
                rtc_get_alarm_2(&temp);
                set_value = (uint8_t) (temp.hr10 * 10 + temp.hr);
                set_value = (uint8_t)((set_value == 0) ? 23 : set_value - 1);
                temp.hr10 = (uint8_t)(set_value/10);
                temp.hr = (uint8_t)(set_value%10);
                rtc_set_alarm_2(&temp);
                break;
            case LCD_CURSOR_ALARM_2_MIN:
                rtc_get_alarm_2(&temp);
                set_value = (uint8_t) (temp.min10 * 10 + temp.min);
                set_value = (uint8_t)((set_value == 0) ? 59 : set_value - 1);
                temp.min10 = (uint8_t)(set_value/10);
                temp.min = (uint8_t)(set_value%10);
                rtc_set_alarm_2(&temp);
                break;
            default:
                happypills_next = TRUE;
        }
        hmi_timer = 0;
        LOG("Pushed DECREASE index: %d", (int) lcd_cursor_get());
    } else if (b3 == DINOUTS_HIGH) {
        /* Set button */
        switch (lcd_cursor_get()) {
            case LCD_CURSOR_TIME_HOUR:
                rtc_get_now(&temp);
                set_value = (uint8_t) (temp.hr10 * 10 + temp.hr);
                set_value = (uint8_t)((set_value >= 23) ? 0 : set_value + 1);
                temp.hr10 = (uint8_t)(set_value/10);
                temp.hr = (uint8_t)(set_value%10);
                rtc_set_now(&temp);
                break;
            case LCD_CURSOR_TIME_MIN:
                rtc_get_now(&temp);
                set_value = (uint8_t) (temp.min10 * 10 + temp.min);
                set_value = (uint8_t)((set_value >= 59) ? 0 : set_value + 1);
                temp.min10 = (uint8_t)(set_value/10);
                temp.min = (uint8_t)(set_value%10);
                rtc_set_now(&temp);
                break;
            case LCD_CURSOR_ALARM_1_HOUR:
                rtc_get_alarm_1(&temp);
                set_value = (uint8_t) (temp.hr10 * 10 + temp.hr);
                set_value = (uint8_t)((set_value >= 23) ? 0 : set_value + 1);
                temp.hr10 = (uint8_t)(set_value/10);
                temp.hr = (uint8_t)(set_value%10);
                rtc_set_alarm_1(&temp);
                break;
            case LCD_CURSOR_ALARM_1_MIN:
                rtc_get_alarm_1(&temp);
                set_value = (uint8_t) (temp.min10 * 10 + temp.min);
                set_value = (uint8_t)((set_value >= 59) ? 0 : set_value + 1);
                temp.min10 = (uint8_t)(set_value/10);
                temp.min = (uint8_t)(set_value%10);
                rtc_set_alarm_1(&temp);
                break;
            case LCD_CURSOR_ALARM_2_HOUR:
                rtc_get_alarm_2(&temp);
                set_value = (uint8_t) (temp.hr10 * 10 + temp.hr);
                set_value = (uint8_t)((set_value >= 23) ? 0 : set_value + 1);
                temp.hr10 = (uint8_t)(set_value/10);
                temp.hr = (uint8_t)(set_value%10);
                rtc_set_alarm_2(&temp);
                break;
            case LCD_CURSOR_ALARM_2_MIN:
                rtc_get_alarm_2(&temp);
                set_value = (uint8_t) (temp.min10 * 10 + temp.min);
                set_value = (uint8_t)((set_value >= 59) ? 0 : set_value + 1);
                temp.min10 = (uint8_t)(set_value/10);
                temp.min = (uint8_t)(set_value%10);
                rtc_set_alarm_2(&temp);
                break;
            default:
                happypills_next = TRUE;
        }
        hmi_timer = 0;
        LOG("Pushed INCREASE index: %d", (int) lcd_cursor_get());
    } else if (b2 == DINOUTS_HIGH) {
        /* Next button */
        lcd_cursor_next();
        hmi_timer = 0;
        LOG("Pushed SET index: %d", (int) lcd_cursor_get());
    }
}

static void hmi_task(uint32_t elapsed) {
    b1 = buttons_read(BUTTONS_1);
    b2 = buttons_read(BUTTONS_2);
    b3 = buttons_read(BUTTONS_3);

    switch (hmi_state) {
        case HMI_STATE_OFF:
            hmi_state_off();
            break;
        case HMI_STATE_ON:
            hmi_state_on();
            break;
        default:
            LOG("Exception, found state %d", hmi_state);
            hmi_state = HMI_STATE_ON;
    }

    /* Update Previous button states */
    b1p = b1;
    b2p = b2;
    b3p = b3;

    /* Increase time */
    hmi_timer += elapsed;
}

hmi_state_t hmi_get_state(void) {
    return hmi_state;
}