
/* Standard C Includes */
#include <stdio.h>
#include <math.h>
#include <xuartlite_l.h>

/* BSP & Xilinx Includes */
#include "xparameters.h"
#include "xgpio.h"
#include "xsysmon.h"
#include "leds.h"

/* Project includes */
#include <platform.h>
#include <scheduler.h>
#include <leds_config.h>
#include <xgpio.h>

static XGpio xGpio1 = {0};
static XGpio xGpio2 = {0};

typedef struct {
    XGpio *xGpio;
    uint32_t channel;
    uint32_t mask;
    uint32_t value;
} leds_descr_t;

static leds_descr_t leds_descr[] = {
        {&xGpio1, 2, 0x01},
        {&xGpio1, 2, 0x02},
        {&xGpio2, 1, 0x00},
        {&xGpio2, 1, 0x02},
        {&xGpio2, 1, 0x00},
};

#ifdef PLATFORM_LEDS_MONITOR_PERIOD

void leds_monitor(u32 elapsed);

static scheduler_entry_t leds_monitor_entry = {0, PLATFORM_LEDS_MONITOR_PERIOD, leds_monitor};

void leds_monitor(u32 elapsed) {
    LOG("Masks: Orange 1: %X; Orange 2: %X; Red: %X; Green: %X; Blue: %X;", (int) leds_descr[0].value,
        (int) leds_descr[1].value, (int) leds_descr[2].value,
        (int) leds_descr[3].value, (int) leds_descr[4].value);
}

#endif /* PLATFORM_LEDS_MONITOR_PERIOD */

int leds_init(void) {
    leds_chan_t ch;
    int ret = 0;

    if (xGpio1.IsReady != 0) {
        return XST_SUCCESS;
    }

    ret = XGpio_Initialize(&xGpio1, PLATFORM_LEDS_GPIO_1);
    if (ret != XST_SUCCESS) {
        LOG_ERROR();
        return ret;
    }

    ret = XGpio_Initialize(&xGpio2, PLATFORM_LEDS_GPIO_2);
    if (ret != XST_SUCCESS) {
        LOG_ERROR();
        return ret;
    }

    scheduler_add_entry(&leds_monitor_entry);

    for (ch = LEDS_CHAN_ORANGE_1; ch <= LEDS_CHAN_BLUE; ch++) {
        leds_turn_off(ch);
    }

    return XST_SUCCESS;
}

void leds_turn_on(leds_chan_t chan) {
    leds_descr_t *led = &leds_descr[chan];
    uint32_t value = XGpio_DiscreteRead(led->xGpio, led->channel) | led->mask;
    XGpio_DiscreteWrite(led->xGpio, led->channel, value);
    led->value = 0x1;
}

void leds_turn_off(leds_chan_t chan) {
    leds_descr_t *led = &leds_descr[chan];
    uint32_t value = XGpio_DiscreteRead(led->xGpio, led->channel) & (~led->mask);
    XGpio_DiscreteWrite(led->xGpio, led->channel, value);
    led->value = 0x0;
}

