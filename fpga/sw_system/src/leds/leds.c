
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
#include <leds_cfg.h>
#include <xgpio.h>

static XGpio xGpio1 = {0};
static XGpio xGpio2 = {0};
static uint32_t xGpio1_mask = 0;
static uint32_t xGpio2_mask = 0;

typedef struct {
    XGpio *xGpio;
    uint32_t channel;
    uint32_t mask;
    uint32_t *value;
    int32_t timeout;
} leds_descr_t;

static leds_descr_t leds_descr[] = {
        {&xGpio1, 2, 0x01, &xGpio1_mask, -1},
        {&xGpio1, 2, 0x02, &xGpio1_mask, -1},
        {&xGpio2, 1, 0x04, &xGpio2_mask, -1},
        {&xGpio2, 1, 0x02, &xGpio2_mask, -1},
        {&xGpio2, 1, 0x01, &xGpio2_mask, -1},
        {(XGpio *) NULL, (uint32_t) NULL, (uint32_t) NULL, (uint32_t*) NULL, (uint32_t) NULL}
};

static void leds_timeout_task(uint32_t elapsed);

static scheduler_entry_t leds_timeout_entry = {0, PLATFORM_LEDS_TIMEOUT_PERIOD, leds_timeout_task};


#ifdef PLATFORM_LEDS_MONITOR_PERIOD

static void leds_monitor(uint32_t elapsed);

static scheduler_entry_t leds_monitor_entry = {0, PLATFORM_LEDS_MONITOR_PERIOD, leds_monitor};

static void leds_monitor(uint32_t elapsed) {
    LOG("Masks: chan 1: %X chan 2: %X;", (int) xGpio1_mask, (int) xGpio2_mask);
}

#endif /* PLATFORM_LEDS_MONITOR_PERIOD */

static void leds_timeout_task(uint32_t elapsed) {
    int i;

    for (i = 0; leds_descr[i].xGpio != NULL; i++) {
        if (leds_descr[i].timeout > 0) {
            leds_descr[i].timeout -= elapsed;

            if (leds_descr[i].timeout <= 0) {
                leds_descr[i].timeout = -1;
                leds_turn_off(i);
            }
        }
    }
}

int leds_init(void) {
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

    scheduler_add_entry(&leds_timeout_entry);

#ifdef PLATFORM_LEDS_MONITOR_PERIOD
    scheduler_add_entry(&leds_monitor_entry);
#endif /* PLATFORM_LEDS_MONITOR_PERIOD */

    xGpio1_mask = 0;
    xGpio2_mask = 0;

    XGpio_DiscreteWrite(&xGpio1, 2, xGpio1_mask);
    XGpio_DiscreteWrite(&xGpio2, 1, xGpio2_mask);

    return XST_SUCCESS;
}

void leds_turn_on(leds_chan_t chan) {
    leds_descr_t *led = &leds_descr[chan];
    uint32_t value = *led->value | led->mask;
    if (chan == LEDS_CHAN_ORANGE_1 || chan == LEDS_CHAN_ORANGE_2) {
        XGpio_DiscreteWrite(led->xGpio, led->channel, value);
    } else {
        XGpio_DiscreteWrite(led->xGpio, led->channel, ~value);
    }
    *led->value = value;
}

void leds_turn_off(leds_chan_t chan) {
    leds_descr_t *led = &leds_descr[chan];
    uint32_t value = *led->value & (~led->mask);
    if (chan == LEDS_CHAN_ORANGE_1 || chan == LEDS_CHAN_ORANGE_2) {
        XGpio_DiscreteWrite(led->xGpio, led->channel, value);
    } else {
        XGpio_DiscreteWrite(led->xGpio, led->channel, ~value);
    }

    *led->value = value;
}

void leds_turn_on_timeout(leds_chan_t chan, uint16_t millis) {
    leds_descr_t *led = &leds_descr[chan];
    leds_turn_on(chan);
    led->timeout = millis;
}
