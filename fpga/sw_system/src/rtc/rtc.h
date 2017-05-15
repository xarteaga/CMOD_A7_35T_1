
#ifndef SW_SYSTEM_RTC_H
#define SW_SYSTEM_RTC_H

typedef struct {
    uint8_t sec:4;
    uint8_t sec10:3;
    uint8_t CH:1;        /* clock halt 1=stop 0=start */

    uint8_t min:4;
    uint8_t min10:3;
    uint8_t zero0:1;

    uint8_t hr:4;
    uint8_t hr10:2;    /* 10 (0-2) or am/pm (am/pm, 0-1) */
    uint8_t zero1:1;
    uint8_t fmt:1;        /* 1=12 hour 0=24 hour */

    uint8_t date:4;
    uint8_t date10:2;
    uint8_t zero2:2;

    uint8_t month:4;
    uint8_t month10:1;
    uint8_t zero3:3;

    uint8_t day:3;        /* day of week */
    uint8_t zero4:5;

    uint8_t year:4;
    uint8_t year10:4;

    uint8_t WP:1;        /* write protect 1=protect 0=unprot */
    uint8_t zero5:7;
} rtc_data_t;

void rtc_init(void);

uint8_t rtc_is_equal (rtc_data_t *data1, rtc_data_t *data2);

char *rtc_get_timestamp_str(rtc_data_t *time);

void rtc_get_now(rtc_data_t *data);

void rtc_decrement_time(rtc_data_t *data, uint32_t seconds);

void rtc_increment_time(rtc_data_t *data, uint32_t seconds);

void rtc_set_now(rtc_data_t *rtc_data);

void rtc_set_alarm_1(rtc_data_t *rtc_data);

void rtc_set_alarm_2(rtc_data_t *rtc_data);

void rtc_get_alarm_1(rtc_data_t *data);

void rtc_get_alarm_2(rtc_data_t *data);

#endif //SW_SYSTEM_RTC_H
