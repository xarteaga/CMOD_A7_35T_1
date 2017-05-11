
/* Standard C Includes */
#include <inttypes.h>

/* BSP & Xilinx Includes */
#include <scheduler.h>
#include <xsysmon.h>
#include "xparameters.h"

/* Project includes */
#include <platform.h>
#include <dinouts.h>
#include "xadc.h"
#include "xadc_cfg.h"

typedef struct {
    uint8_t channel;
    dinouts_pin_t selector_pin;
} xadc_descr_t;

#define XADC_HW_CHANNEL_04 (XSM_CH_AUX_MIN + 0x04)
#define XADC_HW_CHANNEL_12 (XSM_CH_AUX_MIN + 0x0C)

static xadc_descr_t xadc_descr [XADC_CHANNEL_UNDEFINED] = {
        {XADC_HW_CHANNEL_04, PLATFORM_XADC_ANALOG_SWITCH_0},
        {XADC_HW_CHANNEL_12, PLATFORM_XADC_ANALOG_SWITCH_1},
        {XADC_HW_CHANNEL_12, PLATFORM_XADC_ANALOG_SWITCH_2},
        {XADC_HW_CHANNEL_12, PLATFORM_XADC_ANALOG_SWITCH_3}
};

static XSysMon xSysMon = {0};

#ifdef PLATFORM_XADC_MONITOR_INPUT_PERIOD

void xadc_monitor(u32 elapsed);

static scheduler_entry_t xadc_monitor_entry = {0, PLATFORM_XADC_MONITOR_INPUT_PERIOD, xadc_monitor};

void xadc_monitor(u32 elapsed) {
    uint16_t v1 = xadc_read_mv(XADC_CHANNEL_0);
    uint16_t v2 = xadc_read_mv(XADC_CHANNEL_1);

    LOG("ADC 0: %4d mV; ADC 1: %4d mV", (int) v1, (int) v2);
}

#endif /* PLATFORM_XADC_MONITOR_INPUT_PERIOD */

int xadc_init(void) {
    XSysMon_Config *ConfigPtr;
    int Status;
    XSysMon *SysMonInstPtr = &xSysMon;

    if (xSysMon.IsReady != 0) {
        return XST_SUCCESS;
    }

    /*
     * Initialize the SysMon driver.
     */
    ConfigPtr = XSysMon_LookupConfig(XPAR_XADC_WIZ_0_DEVICE_ID);
    if (ConfigPtr == NULL) {
        return XST_FAILURE;
    }
    XSysMon_CfgInitialize(SysMonInstPtr, ConfigPtr,
                          ConfigPtr->BaseAddress);

    /*
     * Self Test the System Monitor/ADC device
     */
    Status = XSysMon_SelfTest(SysMonInstPtr);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * Disable the Channel Sequencer before configuring the Sequence
     * registers.
     */
    XSysMon_SetSequencerMode(SysMonInstPtr, XSM_SEQ_MODE_SAFE);

    /*
     * Enable the following channels in the Sequencer registers:
     * 	- On-chip Temperature, VCCINT/VCCAUX supply sensors
     * 	- 1st to 16th Auxiliary Channel
     *	- Calibration Channel
     */
    Status = XSysMon_SetSeqChEnables(SysMonInstPtr, XSM_SEQ_CH_AUX04 | XSM_SEQ_CH_AUX12);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * Enable the Channel Sequencer in continuous sequencer cycling mode.
     */
    XSysMon_SetSequencerMode(SysMonInstPtr, XSM_SEQ_MODE_CONTINPASS);

#ifdef PLATFORM_XADC_MONITOR_INPUT_PERIOD
    scheduler_add_entry(&xadc_monitor_entry);
#endif /* PLATFORM_XADC_MONITOR_INPUT_PERIOD */

    LOG("OK", "");
    return XST_SUCCESS;
}

u16 xadc_read_raw(xadc_channel_t channel) {
    int i;
    xadc_descr_t xadc;
    uint16_t val = 0xFFFF;

    if (channel >= XADC_CHANNEL_UNDEFINED) {
        return val;
    }

    xadc = xadc_descr[channel];

    /*for (i = 0; i < XADC_CHANNEL_UNDEFINED; i++) {
        if (xadc_descr[i].selector_pin != DINOUTS_UNDEFINED) {
            if ( i == channel) {
                dinouts_turn_on(xadc_descr[i].selector_pin);
            } else {
                dinouts_turn_off(xadc_descr[i].selector_pin);
            }
        }
    }
    sleep(0.01);*/

    XSysMon_GetStatus(&xSysMon); /* Clear the old status */
    while ((XSysMon_GetStatus(&xSysMon) & XSM_SR_BUSY_MASK) ==
           XSM_SR_BUSY_MASK);

    val = XSysMon_GetAdcData(&xSysMon, xadc.channel);

    /*if (xadc.selector_pin != DINOUTS_UNDEFINED) {
        dinouts_turn_off(xadc.selector_pin);
    }*/

    return val;
}

uint16_t xadc_read_mv(xadc_channel_t channel) {
    return (3300 * xadc_read_raw(channel)) >> 15;
}