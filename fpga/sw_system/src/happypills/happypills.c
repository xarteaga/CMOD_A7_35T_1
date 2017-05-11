
/* Standard C Includes */
#include <stdio.h>
#include <inttypes.h>

/* BSP & Xilinx Includes */
#include <xparameters.h>
#include <xgpio.h>
#include <xenv.h>

/* Project includes */
#include <platform.h>
#include <scheduler.h>
#include <dinouts.h>
#include <actuators.h>
#include <irproximity.h>
#include <buttons.h>
#include <rtc.h>
#include "happypills_controller.h"
#include "happypills_hmi.h"

static void happypills_task(uint32_t elapsed);

static void happypills_display_task(uint32_t elapsed);

static rtc_data_t happypills_rtc_time;
static uint32_t happypills_next = FALSE;

void happypills_init(void) {
    happypills_controller_init();
    happypills_hmi_init();

    LOG_OK();
}


