
/* Standard C Includes */
#include <stdio.h>

/* BSP & Xilinx Includes */
#include <xparameters.h>
#include <xstatus.h>

/* Project includes */
#include <platform.h>
#include <scheduler.h>
#include <rtc_cfg.h>
#include <xenv.h>
#include "rtc_ds1302.h"
#include "rtc.h"

#define RTC_TO_BCD_H(X) ((uint8_t)(X)/10)
#define RTC_TO_BCD_L(X) ((uint8_t)(X)%10)

static char rtc_ds1302_timestamp_rtc[18] = {0};
static rtc_data_t rtc_ds1302_time = {0};
static rtc_data_t rtc_alarm_1 = {0};
static rtc_data_t rtc_alarm_2 = {0};


static void rtc_time_update (uint32_t elapsed);
static scheduler_entry_t rtc_time_update_entry = {0, PLATFORM_RTC_MONITOR_PERIOD, rtc_time_update};

static void rtc_time_update (uint32_t elapsed) {
    rtc_ds1302_clk_read(&rtc_ds1302_time);
}

#ifdef PLATFORM_RTC_MONITOR_PERIOD

static void rtc_monitor(uint32_t elapsed);

static scheduler_entry_t rtc_monitor_entry = {0, PLATFORM_RTC_MONITOR_PERIOD, rtc_monitor};

static void rtc_monitor(uint32_t elapsed) {
    LOG("Time: %d%d:%d%d:%d%d Date: %d%d/%d%d/%d%d", (int) rtc_ds1302_time.hr10, (int) rtc_ds1302_time.hr, (int) rtc_ds1302_time.min10,
        (int) rtc_ds1302_time.min, (int) rtc_ds1302_time.sec10, (int) rtc_ds1302_time.sec, (int) rtc_ds1302_time.date10, (int) rtc_ds1302_time.date,
        (int) rtc_ds1302_time.month10, (int) rtc_ds1302_time.month, (int) rtc_ds1302_time.year10, (int) rtc_ds1302_time.year);
}

#endif /* PLATFORM_RTC_MONITOR_PERIOD */

void rtc_init(void) {
    rtc_data_t data[2];

    rtc_ds1302_init();

    rtc_ds1302_time.CH = 0;
    rtc_ds1302_time.fmt = 0;
    rtc_ds1302_time.WP = 0;
#if PLATFORM_RTC_SET_TEST_TIME == 1
    rtc_ds1302_time.sec = 6;
    rtc_ds1302_time.sec10 = 5;
    rtc_ds1302_time.min = 9;
    rtc_ds1302_time.min10 = 5;
    rtc_ds1302_time.hr = 3;
    rtc_ds1302_time.hr10 = 2;
    rtc_ds1302_time.date = 1;
    rtc_ds1302_time.date10 = 3;
    rtc_ds1302_time.month = 2;
    rtc_ds1302_time.month10 = 1;
    rtc_ds1302_time.year = 7;
    rtc_ds1302_time.year10 = 1;

    rtc_ds1302_write(&rtc_ds1302_time);
#elif PLATFORM_RTC_SET_NOW == 1
    rtc_ds1302_time.sec = RTC_TO_BCD_L(PLATFORM_RTC_NOW_SECONDS);
    rtc_ds1302_time.sec10 = RTC_TO_BCD_H(PLATFORM_RTC_NOW_SECONDS);
    rtc_ds1302_time.min = RTC_TO_BCD_L(PLATFORM_RTC_NOW_MINUTES);
    rtc_ds1302_time.min10 = RTC_TO_BCD_H(PLATFORM_RTC_NOW_MINUTES);
    rtc_ds1302_time.hr = RTC_TO_BCD_L(PLATFORM_RTC_NOW_HOURS);
    rtc_ds1302_time.hr10 = RTC_TO_BCD_H(PLATFORM_RTC_NOW_HOURS);
    rtc_ds1302_time.date = RTC_TO_BCD_L(PLATFORM_RTC_NOW_DAY);
    rtc_ds1302_time.date10 = RTC_TO_BCD_H(PLATFORM_RTC_NOW_DAY);
    rtc_ds1302_time.month = RTC_TO_BCD_L(PLATFORM_RTC_NOW_MONTH);
    rtc_ds1302_time.month10 = RTC_TO_BCD_H(PLATFORM_RTC_NOW_MONTH);
    rtc_ds1302_time.year = RTC_TO_BCD_L(PLATFORM_RTC_NOW_YEAR);
    rtc_ds1302_time.year10 = RTC_TO_BCD_H(PLATFORM_RTC_NOW_YEAR);

    rtc_ds1302_clk_write(&rtc_ds1302_time);
#endif /* PLATFORM_RTC_SET_TEST_TIME */

#ifdef PLATFORM_RTC_MONITOR_PERIOD
    scheduler_add_entry(&rtc_monitor_entry);
#endif /* PLATFORM_RTC_MONITOR_PERIOD */

    scheduler_add_entry(&rtc_time_update_entry);

    rtc_ds1302_ram_read(data, 2);
    memcpy(&rtc_alarm_1, &data[0*sizeof(rtc_data_t)], sizeof(rtc_data_t));
    memcpy(&rtc_alarm_2, &data[1*sizeof(rtc_data_t)], sizeof(rtc_data_t));

    LOG_OK();
}

char *rtc_get_timestamp_str(rtc_data_t *time) {
    memset(rtc_ds1302_timestamp_rtc, 0, sizeof(rtc_ds1302_timestamp_rtc));

    rtc_ds1302_timestamp_rtc[0] = (char) (0x30 + time->sec10);
    rtc_ds1302_timestamp_rtc[1] = (char) (0x30 + time->sec);
    rtc_ds1302_timestamp_rtc[2] = ':';
    rtc_ds1302_timestamp_rtc[3] = (char) (0x30 + time->min10);
    rtc_ds1302_timestamp_rtc[4] = (char) (0x30 + time->min);
    rtc_ds1302_timestamp_rtc[5] = ':';
    rtc_ds1302_timestamp_rtc[6] = (char) (0x30 + time->sec10);
    rtc_ds1302_timestamp_rtc[7] = (char) (0x30 + time->sec);
    rtc_ds1302_timestamp_rtc[8] = ' ';
    rtc_ds1302_timestamp_rtc[9] = (char) (0x30 + time->date10);
    rtc_ds1302_timestamp_rtc[10] = (char) (0x30 + time->date);
    rtc_ds1302_timestamp_rtc[11] = '/';
    rtc_ds1302_timestamp_rtc[12] = (char) (0x30 + time->month10);
    rtc_ds1302_timestamp_rtc[13] = (char) (0x30 + time->month);
    rtc_ds1302_timestamp_rtc[14] = '/';
    rtc_ds1302_timestamp_rtc[15] = (char) (0x30 + time->year10);
    rtc_ds1302_timestamp_rtc[16] = (char) (0x30 + time->year);
    rtc_ds1302_timestamp_rtc[17] = '\0';

    return rtc_ds1302_timestamp_rtc;
}

rtc_data_t *rtc_get_now(void) {
    return &rtc_ds1302_time;
}

void rtc_set_now(rtc_data_t *rtc_data) {
    memcpy(&rtc_ds1302_time, rtc_data, sizeof(rtc_data_t));
    rtc_ds1302_clk_write(rtc_data);
}

void rtc_set_alarm_1(rtc_data_t *rtc_data) {
    rtc_data_t data[2];
    memcpy(&rtc_alarm_1, rtc_data, sizeof(rtc_data_t));
    memcpy(&data[0], &rtc_alarm_1, sizeof(rtc_data_t));
    memcpy(&data[1], &rtc_alarm_2, sizeof(rtc_data_t));
    rtc_ds1302_ram_write(data, 2);
}

void rtc_set_alarm_2(rtc_data_t *rtc_data) {
    rtc_data_t data[2];
    memcpy(&rtc_alarm_2, rtc_data, sizeof(rtc_data_t));
    memcpy(&data[0], &rtc_alarm_1, sizeof(rtc_data_t));
    memcpy(&data[1], &rtc_alarm_2, sizeof(rtc_data_t));
    rtc_ds1302_ram_write(data, 2);
}

rtc_data_t *rtc_get_alarm_1 (void) {
    return &rtc_alarm_1;
}

rtc_data_t *rtc_get_alarm_2 (void) {
    return &rtc_alarm_2;
}