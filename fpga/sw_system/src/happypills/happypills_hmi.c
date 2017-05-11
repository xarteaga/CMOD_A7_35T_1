
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

typedef enum {
    HMI_STATE_UNDEFINED = -1,
    HMI_STATE_OFF = 0,
    HMI_STATE_ON = 1,
    HMI_STATE_SETTING = 2
} hmi_state_t;

static void hmi_task(uint32_t elapsed);

static scheduler_entry_t hmi_task_entry = {0, 10, hmi_task};
static hmi_state_t hmi_state = HMI_STATE_UNDEFINED;
static uint32_t hmi_timer = 0;
static uint8_t happypills_next = FALSE;
static uint8_t happypills_blink = FALSE;

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

    hmi_state = HMI_STATE_OFF;
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
        lcd_set_backlight(0x1D);
        hmi_timer = 0;
        hmi_state = HMI_STATE_ON;
    }

    lcd_set_mode(LCD_MODE_STANDBY);
}

static void hmi_state_on(void) {
    rtc_data_t *temp;

    if ((b1 == DINOUTS_HIGH && b1 == b1p) ||
        (b2 == DINOUTS_HIGH && b2 == b2p) ||
        (b3 == DINOUTS_HIGH && b3 == b3p)) {
        /* If button has not been release: do nothing */
    } else if (hmi_timer >= 10000) {
        /* Go to OFF if timeout without pushing any button */
        hmi_timer = 0;
        lcd_set_backlight(0);
        lcd_cursor_reset();
        hmi_state = HMI_STATE_OFF;
        LOG("Buttons timeout %d", (int) lcd_cursor_get());
    } else if (b1 == DINOUTS_HIGH) {
        /* Previous button */
        hmi_timer = 0;
        lcd_cursor_previous();
        LOG("Pushed PREV index: %d", (int) lcd_cursor_get());
    } else if (b2 == DINOUTS_HIGH) {
        /* Set button */
        switch (lcd_cursor_get()) {
            case LCD_CURSOR_TIME_HOUR:
                temp = rtc_get_now();
                set_value = (uint8_t) (temp->hr10 * 10 + temp->hr);
                hmi_state = HMI_STATE_SETTING;
                break;
            case LCD_CURSOR_TIME_MIN:
                temp = rtc_get_now();
                set_value = (uint8_t) (temp->min10 * 10 + temp->min);
                hmi_state = HMI_STATE_SETTING;
                break;
            case LCD_CURSOR_ALARM_1_HOUR:
                temp = rtc_get_alarm_1();
                set_value = (uint8_t) (temp->hr10 * 10 + temp->hr);
                hmi_state = HMI_STATE_SETTING;
                break;
            case LCD_CURSOR_ALARM_1_MIN:
                temp = rtc_get_alarm_1();
                set_value = (uint8_t) (temp->min10 * 10 + temp->min);
                hmi_state = HMI_STATE_SETTING;
                break;
            case LCD_CURSOR_ALARM_2_HOUR:
                temp = rtc_get_alarm_2();
                set_value = (uint8_t) (temp->hr10 * 10 + temp->hr);
                hmi_state = HMI_STATE_SETTING;
                break;
            case LCD_CURSOR_ALARM_2_MIN:
                temp = rtc_get_alarm_2();
                set_value = (uint8_t) (temp->min10 * 10 + temp->min);
                hmi_state = HMI_STATE_SETTING;
                break;
            default:
                /* Do Nothing */;
        }
        hmi_timer = 0;
        LOG("Pushed SET index: %d", (int) lcd_cursor_get());
    } else if (b3 == DINOUTS_HIGH) {
        /* Next button */
        lcd_cursor_next();
        hmi_timer = 0;
        LOG("Pushed NEXT index: %d", (int) lcd_cursor_get());
    }

    rtc_data_t *now = rtc_get_now();
    //LOG("NOW: %s", rtc_get_timestamp_str(now));
    //lcd_set_time(now);
    //lcd_set_alarm1(rtc_get_alarm_1());
    //lcd_set_alarm2(rtc_get_alarm_2());

    lcd_set_mode(LCD_MODE_SELECT);
}

static void hmi_state_setting(void) {
    /* Continous push counters (up to ten) */
    static uint8_t b1count = 0;
    static uint8_t b3count = 0;
    rtc_data_t *temp;

    if (hmi_timer > 500) {
        if (happypills_blink == TRUE) {
            happypills_blink = FALSE;
        } else {
            happypills_blink = TRUE;
        }
        hmi_timer = 0;
    }

    if (b1 == DINOUTS_HIGH && b1 == b1p) {
        b1count++;
        b3count = 0;
    } else if (b3 == DINOUTS_HIGH && b3 == b3p) {
        b1count = 0;
        b3count++;
    } else if (b2 == DINOUTS_HIGH && b2p == DINOUTS_LOW) {
        hmi_state = HMI_STATE_ON;
        happypills_blink = FALSE;
        hmi_state = HMI_STATE_SETTING;
        switch (lcd_cursor_get()) {
            case LCD_CURSOR_TIME_HOUR:
                temp = rtc_get_now();
                temp->hr10 = (uint8_t)(set_value/10);
                temp->hr = (uint8_t)(set_value%10);
                rtc_set_now(temp);
                break;
            case LCD_CURSOR_TIME_MIN:
                temp = rtc_get_now();
                temp->min10 = (uint8_t)(set_value/10);
                temp->min = (uint8_t)(set_value%10);
                rtc_set_now(temp);
                break;
            case LCD_CURSOR_ALARM_1_HOUR:
                temp = rtc_get_alarm_1();
                temp->hr10 = (uint8_t)(set_value/10);
                temp->hr = (uint8_t)(set_value%10);
                rtc_set_alarm_1(temp);
                break;
            case LCD_CURSOR_ALARM_1_MIN:
                temp = rtc_get_alarm_1();
                temp->min10 = (uint8_t)(set_value/10);
                temp->min = (uint8_t)(set_value%10);
                rtc_set_alarm_1(temp);
                break;
            case LCD_CURSOR_ALARM_2_HOUR:
                temp = rtc_get_alarm_2();
                temp->hr10 = (uint8_t)(set_value/10);
                temp->hr = (uint8_t)(set_value%10);
                rtc_set_alarm_2(temp);
                break;
            case LCD_CURSOR_ALARM_2_MIN:
                temp = rtc_get_alarm_2();
                temp->min10 = (uint8_t)(set_value/10);
                temp->min = (uint8_t)(set_value%10);
                rtc_set_alarm_2(temp);
                break;
            default:
                /* Do nothing */;
        }

        lcd_set_mode(LCD_MODE_SET);
    }

    if (b1count >= 10) {
        switch (lcd_cursor_get()) {
            case LCD_CURSOR_TIME_HOUR:
            case LCD_CURSOR_ALARM_1_HOUR:
            case LCD_CURSOR_ALARM_2_HOUR:
                set_value = (uint8_t)((set_value == 23) ? 0 : set_value + 1);
                break;
            case LCD_CURSOR_TIME_MIN:
            case LCD_CURSOR_ALARM_1_MIN:
            case LCD_CURSOR_ALARM_2_MIN:
            default:
                set_value = (uint8_t)((set_value == 59) ? 0 : set_value + 1);
        }
        b1count = 0;
    } else if (b3count >= 10) {
        switch (lcd_cursor_get()) {
            case LCD_CURSOR_TIME_HOUR:
            case LCD_CURSOR_ALARM_1_HOUR:
            case LCD_CURSOR_ALARM_2_HOUR:
                set_value = (uint8_t)((set_value == 0) ? 23 : set_value - 1);
                break;
            case LCD_CURSOR_TIME_MIN:
            case LCD_CURSOR_ALARM_1_MIN:
            case LCD_CURSOR_ALARM_2_MIN:
            default:
                set_value = (uint8_t)((set_value == 0) ? 59 : set_value - 1);
        }
        b3count = 0;
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
        case HMI_STATE_SETTING:
            hmi_state_setting();
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