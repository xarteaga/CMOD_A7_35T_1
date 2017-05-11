
#ifndef SW_SYSTEM_RTC_DS1302_H
#define SW_SYSTEM_RTC_DS1302_H

#include "rtc.h"

void rtc_ds1302_init (void);
void rtc_ds1302_clk_write(rtc_data_t *data);
void rtc_ds1302_clk_read(rtc_data_t *data);
void rtc_ds1302_ram_write(rtc_data_t *data, uint8_t count);
void rtc_ds1302_ram_read(rtc_data_t *data, uint8_t count);

#endif //SW_SYSTEM_RTC_DS1302_H
