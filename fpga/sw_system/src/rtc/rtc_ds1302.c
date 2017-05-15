
/* Standard C Includes */
#include <stdio.h>

/* BSP & Xilinx Includes */
#include <xparameters.h>
#include <xstatus.h>
#include <xenv.h>
#include <string.h>

/* Project includes */
#include <platform.h>
#include <rtc_cfg.h>
#include <dinouts.h>
#include "rtc_ds1302.h"
#include "rtc.h"

#define RTC_DS1302_CLK_READ  0xBF
#define RTC_DS1302_CLK_WRITE 0xBE
#define RTC_DS1302_RAM_READ  0xFF
#define RTC_DS1302_RAM_WRITE 0xFE

static void rtc_ds1302_start(void) {
    dinouts_turn_off(PLATFORM_RTC_DS1302_CLK);
    dinouts_turn_on(PLATFORM_RTC_DS1302_CE);
    udelay(4);
}

static void rtc_ds1302_stop(void) {
    dinouts_turn_off(PLATFORM_RTC_DS1302_CLK);
    dinouts_turn_off(PLATFORM_RTC_DS1302_CE);
    udelay(4);
}

static void rtc_ds1302_write_byte(uint8_t byte) {
    dinouts_set_direction(PLATFORM_RTC_DS1302_DATA, DINOUTS_DIRECTION_OUT);

    dinouts_shift_out(PLATFORM_RTC_DS1302_DATA, PLATFORM_RTC_DS1302_CLK, TRUE, byte);
}

static uint8_t rtc_ds1302_read_byte(void) {
    int i;
    uint8_t input_value = 0;
    uint8_t bit = 0;

    dinouts_set_direction(PLATFORM_RTC_DS1302_DATA, DINOUTS_DIRECTION_IN);

    for (i = 0; i < 8; ++i) {
        bit = dinouts_read(PLATFORM_RTC_DS1302_DATA);
        input_value |= (bit << i);  // Bits are read LSB first.

        // See the note in writeOut() about timing. digitalWrite() is slow enough to
        // not require extra delays for tCH and tCL.
        dinouts_turn_on(PLATFORM_RTC_DS1302_CLK);
        dinouts_turn_off(PLATFORM_RTC_DS1302_CLK);
    }
    return input_value;
}

void rtc_ds1302_init(void) {
    dinouts_init();

    dinouts_turn_off(PLATFORM_RTC_DS1302_CE);
    dinouts_turn_on(PLATFORM_RTC_DS1302_VCC);
    dinouts_turn_off(PLATFORM_RTC_DS1302_GND);

}

static void rtc_ds1302_write(uint8_t mode, rtc_data_t *data, uint8_t size) {
    int i;
    rtc_ds1302_start();

    rtc_ds1302_write_byte(mode);

    for (i = 0; i < size; i++) {
        rtc_ds1302_write_byte(((uint8_t *) data)[i]);
    }

    rtc_ds1302_stop();
}

static void rtc_ds1302_read(uint8_t mode, rtc_data_t *data, uint8_t size) {
    int i;
    rtc_ds1302_start();

    rtc_ds1302_write_byte(mode);

    for (i = 0; i < size; i++) {
        ((uint8_t *) data)[i] = rtc_ds1302_read_byte();
    }

    rtc_ds1302_stop();
}

void rtc_ds1302_clk_write(rtc_data_t *data) {
    rtc_ds1302_write(RTC_DS1302_CLK_WRITE, data, sizeof(rtc_data_t));
}

void rtc_ds1302_clk_read(rtc_data_t *data) {
    rtc_ds1302_read(RTC_DS1302_CLK_READ, data, sizeof(rtc_data_t));
}

void rtc_ds1302_ram_write(uint8_t *data, uint8_t count) {
    rtc_ds1302_write(RTC_DS1302_RAM_WRITE, data, count);
}

void rtc_ds1302_ram_read(uint8_t *data, uint8_t count) {
    rtc_ds1302_read(RTC_DS1302_RAM_READ, data, count);
}

