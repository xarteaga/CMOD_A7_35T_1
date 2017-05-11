
#ifndef SW_SYSTEM_LCD_H
#define SW_SYSTEM_LCD_H

typedef enum {
    LCD_CURSOR_NONE = -1,
    LCD_CURSOR_TIME_HOUR = 0,
    LCD_CURSOR_TIME_MIN = 1,
    LCD_CURSOR_ALARM_1_HOUR = 2,
    LCD_CURSOR_ALARM_1_MIN = 3,
    LCD_CURSOR_ALARM_2_HOUR = 4,
    LCD_CURSOR_ALARM_2_MIN = 5,
    LCD_CURSOR_UNDEFINED = 6
} lcd_cursor_t;

typedef enum {
    LCD_MODE_STANDBY = 0,
    LCD_MODE_SELECT = 1,
    LCD_MODE_SET = 2
} lcd_mode_t;


void lcd_init(void);

void lcd_set_backlight(uint8_t backlight);

void lcd_set_time(rtc_data_t *t);

void lcd_set_alarm1(rtc_data_t *t);

void lcd_set_alarm2(rtc_data_t *t);

void lcd_update(void);

lcd_cursor_t lcd_cursor_get(void);

void lcd_cursor_next(void);

void lcd_cursor_previous(void);

void lcd_cursor_reset(void);

void lcd_set_mode (lcd_mode_t m);

#endif //SW_SYSTEM_LCD_H
