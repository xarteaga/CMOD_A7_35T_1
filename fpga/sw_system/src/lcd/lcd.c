
/* Standard C Includes */
#include <stdio.h>
#include <inttypes.h>

/* BSP & Xilinx Includes */
#include <xparameters.h>
#include <xenv.h>
#include <xuartlite.h>

/* Project includes */
#include <platform.h>
#include <scheduler.h>
#include <lcd_cfg.h>
#include <rtc.h>
#include "lcd.h"


/*********************
 * 0123456789abcdef *
 *+-----+----------+*
 *|HORA | A1: 04:00|*
 *|22:12| A2: 21:00|*
 *+-----+----------+*
 ********************
 */
static uint8_t lcd_uart_busy = FALSE;
static uint8_t lcd_target_backlight = 0;
static rtc_data_t time = {0};
static rtc_data_t alarm1 = {0};
static rtc_data_t alarm2 = {0};
static uint8_t time_update = FALSE;
static uint8_t alarm1_update = FALSE;
static uint8_t alarm2_update = FALSE;
static lcd_cursor_t cursor = LCD_CURSOR_NONE;
static lcd_mode_t mode = LCD_MODE_STANDBY;
XUartLite lcd_uart;

static void lcd_update_task(uint32_t elapsed);

static scheduler_entry_t lcd_update_entry = {0, 5, lcd_update_task};

static void lcd_uart_rx_handler(void *CallBackRef, unsigned int EventData) {
    return;
}

static void lcd_uart_tx_handler(void *CallBackRef, unsigned int EventData) {
    return;
}

static void lcd_update_task(uint32_t elapsed) {
    static uint8_t buffer[64] = {0};
    static uint8_t state_blink = FALSE;
    static uint32_t timer_blink = 0;
    static uint32_t timer_update = 0;
    static uint8_t lcd_current_backlight = 0;
    uint8_t update = FALSE;
    uint8_t m = (uint8_t) ((mode == LCD_MODE_SELECT) ? 0x0E : (mode == LCD_MODE_SET) ? 0x0D : 0x0C);
    uint8_t c = 0;
    uint8_t i = 0;

    /* Manage Backlight */
    if (lcd_current_backlight != lcd_target_backlight) {
        if (lcd_current_backlight > lcd_target_backlight) {
            lcd_current_backlight--;
        } else if (lcd_current_backlight < lcd_target_backlight) {
            lcd_current_backlight++;
        }
        buffer[i++] = 0x7C;
        buffer[i++] = (uint8_t) (0x80 + lcd_current_backlight);
        XUartLite_Send(&lcd_uart, buffer, i);
        timer_blink = 0;
        timer_update = 0;
        LOG_OK();
        return;
    }

    if (timer_blink > 500) {
        state_blink = (uint8_t) ((state_blink == TRUE) ? FALSE : TRUE);
        timer_blink = 0;
    } else {
        timer_blink += elapsed; // Increase timer
    }

    if (timer_update > 1000) {
        update = TRUE;
        timer_update = 0;
    } else {
        timer_update += elapsed;
    }

    /* Select cursor position */
    switch (cursor) {
        case LCD_CURSOR_TIME_HOUR:
            c = 0x41;
            break;
        case LCD_CURSOR_TIME_MIN:
            c = 0x44;
            break;
        case LCD_CURSOR_ALARM_1_HOUR:
            c = 0x0C;
            break;
        case LCD_CURSOR_ALARM_1_MIN:
            c = 0x0F;
            break;
        case LCD_CURSOR_ALARM_2_HOUR:
            c = 0x4C;
            break;
        case LCD_CURSOR_ALARM_2_MIN:
            c = 0x4F;
            break;
        default:
            c = 0x00;
            m = 0x0C;
    }

    /* Set Cursor on first line */
    if (update == TRUE) {
        buffer[i++] = 0xFE;
        buffer[i++] = 0x80 + 0x00;
        buffer[i++] = 'H';
        buffer[i++] = 'o';
        buffer[i++] = 'r';
        buffer[i++] = 'a';
        buffer[i++] = ' ';
        buffer[i++] = ' ';
        buffer[i++] = ' ';
        buffer[i++] = 'A';
        buffer[i++] = '1';
        buffer[i++] = ':';
        buffer[i++] = ' ';
    }

    /* Alarm 1 */
    if (timer_blink == 0 || alarm1_update == TRUE) {
        buffer[i++] = 0xFE;
        buffer[i++] = 0x80 + 0x0B;
        buffer[i++] = (uint8_t) ((cursor == LCD_CURSOR_ALARM_1_HOUR && state_blink == TRUE)?' ':(alarm1.hr10 + 0x30));
        buffer[i++] = (uint8_t) ((cursor == LCD_CURSOR_ALARM_1_HOUR && state_blink == TRUE)?' ':(alarm1.hr + 0x30));
        buffer[i++] = ':';
        buffer[i++] = (uint8_t) ((cursor == LCD_CURSOR_ALARM_1_MIN && state_blink == TRUE)?' ':(alarm1.min10 + 0x30));
        buffer[i++] = (uint8_t) ((cursor == LCD_CURSOR_ALARM_1_MIN && state_blink == TRUE)?' ':(alarm1.min + 0x30));
        alarm1_update = FALSE;
    }

    /* Time */
    if (timer_blink == 0 || time_update == TRUE) {
        buffer[i++] = 0xFE;
        buffer[i++] = 0x80 + 0x40;
        buffer[i++] = (uint8_t) ((cursor == LCD_CURSOR_TIME_HOUR && state_blink == TRUE)?' ':(time.hr10 + 0x30));
        buffer[i++] = (uint8_t) ((cursor == LCD_CURSOR_TIME_HOUR && state_blink == TRUE)?' ':(time.hr + 0x30));
        buffer[i++] = (uint8_t) ((state_blink == TRUE && cursor == LCD_CURSOR_NONE) ? ' ' : ':');
        buffer[i++] = (uint8_t) ((cursor == LCD_CURSOR_TIME_MIN && state_blink == TRUE)?' ':(time.min10 + 0x30));
        buffer[i++] = (uint8_t) ((cursor == LCD_CURSOR_TIME_MIN && state_blink == TRUE)?' ':(time.min + 0x30));
        time_update = FALSE;
    }

    if (update == TRUE) {
        buffer[i++] = 0xFE;
        buffer[i++] = 0x80 + 0x45;
        buffer[i++] = ' ';
        buffer[i++] = ' ';
        buffer[i++] = 'A';
        buffer[i++] = '2';
        buffer[i++] = ':';
        buffer[i++] = ' ';
    }

    /* Alarm 2 */
    if (timer_blink == 0 || alarm2_update == TRUE) {
        buffer[i++] = 0xFE;
        buffer[i++] = 0x80 + 0x4B;
        buffer[i++] = (uint8_t) ((cursor == LCD_CURSOR_ALARM_2_HOUR && state_blink == TRUE)?' ':(alarm2.hr10 + 0x30));
        buffer[i++] = (uint8_t) ((cursor == LCD_CURSOR_ALARM_2_HOUR && state_blink == TRUE)?' ':(alarm2.hr + 0x30));
        buffer[i++] = ':';
        buffer[i++] = (uint8_t) ((cursor == LCD_CURSOR_ALARM_2_MIN && state_blink == TRUE)?' ':(alarm2.min10 + 0x30));
        buffer[i++] = (uint8_t) ((cursor == LCD_CURSOR_ALARM_2_MIN && state_blink == TRUE)?' ':(alarm2.min + 0x30));
        alarm2_update = FALSE;
    }

    /* Set Cursor on desired position */

    /* Stop Mark */
    /*buffer[i++] = 0xFE;
    buffer[i++] = m;*/

    if (i > 0) {
        buffer[i++] = 0xFE;
        buffer[i++] = (uint8_t) (0x80 + c);
        //LOG("Updating %5d %5d", timer_blink, i);
        XUartLite_Send(&lcd_uart, buffer, i);
    }
}

static void lcd_reset(void) {
    uint8_t rst_buf[1] = {0x12};
    XUartLite_Send(&lcd_uart, rst_buf, sizeof(rst_buf));
    LOG("Reseting LCD%s", "");
}

void lcd_init(void) {
    int status = XUartLite_Initialize(&lcd_uart, PLATFORM_LCD_DEVICE_ID);
    XUartLite_SetRecvHandler(&lcd_uart, lcd_uart_rx_handler, &lcd_uart);
    XUartLite_SetSendHandler(&lcd_uart, lcd_uart_tx_handler, &lcd_uart);
    XUartLite_EnableInterrupt(&lcd_uart);

    if (status != XST_SUCCESS) {
        LOG("Error initiating UART (%d)", status);
    }
    lcd_uart_busy = FALSE;
    lcd_reset();
    lcd_set_backlight(0);

    scheduler_add_entry(&lcd_update_entry);

    LOG_OK();
}

void lcd_set_backlight(uint8_t backlight) {

    if (backlight > 0x1D) {
        backlight = 0x1D;
    }
    lcd_target_backlight = backlight;
}

void lcd_set_time(rtc_data_t *t) {
    if (memcmp(t, &time, sizeof(rtc_data_t)) == FALSE){
        memcpy(&time, t, sizeof(rtc_data_t));
        time_update = TRUE;
        LOG("LCD time change: %s", rtc_get_timestamp_str(t));
    }
}

void lcd_set_alarm1(rtc_data_t *t) {
    if (memcmp(t, &alarm1, sizeof(rtc_data_t)) == FALSE) {
        memcpy(&alarm1, t, sizeof(rtc_data_t));
        alarm1_update = TRUE;
    }
}

void lcd_set_alarm2(rtc_data_t *t) {
    if (memcmp(t, &alarm2, sizeof(rtc_data_t)) == FALSE) {
        memcpy(&alarm2, t, sizeof(rtc_data_t));
        alarm2_update = TRUE;
    }
}

lcd_cursor_t lcd_cursor_get(void) {
    return cursor;
}

void lcd_cursor_next(void) {
    if (cursor == LCD_CURSOR_ALARM_2_MIN) {
        cursor = LCD_CURSOR_TIME_HOUR;
    } else {
        cursor++;
    }
}

void lcd_cursor_previous(void) {
    if (cursor <= LCD_CURSOR_TIME_HOUR) {
        cursor = LCD_CURSOR_ALARM_2_MIN;
    } else {
        cursor--;
    }
}

void lcd_cursor_reset(void) {
    cursor = LCD_CURSOR_NONE;
}

void lcd_set_mode(lcd_mode_t m) {
    mode = m;
}