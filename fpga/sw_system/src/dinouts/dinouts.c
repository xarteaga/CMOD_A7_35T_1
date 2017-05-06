
/* Standard C Includes */
#include <stdio.h>
#include <inttypes.h>

/* BSP & Xilinx Includes */
#include <xparameters.h>
#include <xgpio.h>

/* Project includes */
#include <platform.h>
#include <scheduler.h>
#include "dinouts_cfg.h"
#include "dinouts.h"

static XGpio xGpioInOuts = {0};
static uint32_t dinouts_write_mask = 0;
static uint32_t dinouts_direction = 0;

#ifdef PLATFORM_DINOUTS_MONITOR_PERIOD

static void dinouts_monitor_task (uint32_t elapsed);

static scheduler_entry_t dinouts_monitor_entry = {0, PLATFORM_DINOUTS_MONITOR_PERIOD, dinouts_monitor_task};

static void dinouts_monitor_task (uint32_t elapsed) {
    uint32_t read_mask = XGpio_DiscreteRead(&xGpioInOuts, 1);
    LOG("Direction mask: %04X Read mask: %04X; Write mask: %04X;", dinouts_direction, read_mask, dinouts_write_mask);
}

#endif /* PLATFORM_DINOUTS_MONITOR_PERIOD */

int dinouts_init(void) {
    int ret = 0;

    if (xGpioInOuts.IsReady != 0) {
        return XST_SUCCESS;
    }

    ret = XGpio_Initialize(&xGpioInOuts, PLATFORM_DINOUTS_DEVICE_ID);
    if (ret != XST_SUCCESS) {
        LOG_ERROR();
        return ret;
    }

#ifdef PLATFORM_DINOUTS_MONITOR_PERIOD
    scheduler_add_entry(&dinouts_monitor_entry);
#endif /* PLATFORM_DINOUTS_MONITOR_PERIOD */

    XGpio_DiscreteWrite(&xGpioInOuts, 1, dinouts_write_mask);
    XGpio_SetDataDirection(&xGpioInOuts, 1, dinouts_direction);

    LOG_OK();
    return XST_SUCCESS;
}

void dinouts_set_direction (dinouts_pin_t chan, dinouts_direction_t direction) {

    if (chan < DINOUTS_PIN_UNKNOWN) {
        dinouts_direction = XGpio_GetDataDirection(&xGpioInOuts, 1);

        if (direction == DINOUTS_DIRECTION_OUT) {
            dinouts_direction = dinouts_direction & (~(1 << chan));
        } else if (direction == DINOUTS_DIRECTION_IN) {
            dinouts_direction = dinouts_direction | (1 << chan);
        }

        XGpio_SetDataDirection(&xGpioInOuts, 1, dinouts_direction);
    }
}

void dinouts_turn_on (dinouts_pin_t chan) {
    if (chan < DINOUTS_PIN_UNKNOWN) {
        dinouts_write_mask = dinouts_write_mask | (1<<chan);
        XGpio_DiscreteWrite(&xGpioInOuts, 1, dinouts_write_mask);
    }
}

void dinouts_turn_off (dinouts_pin_t chan) {
    if (chan < DINOUTS_PIN_UNKNOWN) {
        dinouts_write_mask = dinouts_write_mask & (~(1 << chan));
        XGpio_DiscreteWrite(&xGpioInOuts, 1, dinouts_write_mask);
    }
}

dinouts_level_t dinouts_read (dinouts_pin_t chan) {
    uint32_t read_mask = XGpio_DiscreteRead(&xGpioInOuts, 1);
    if (chan < DINOUTS_PIN_UNKNOWN) {
        return (dinouts_level_t)(read_mask >> chan) & 0x01;
    }
    return DINOUTS_UNDEFINED;
}