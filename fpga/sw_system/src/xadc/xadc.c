
/* Standard C Includes */

/* BSP & Xilinx Includes */
#include <scheduler.h>
#include <xsysmon.h>
#include "xparameters.h"
#include "xsysmon.h"

/* Project includes */
#include "platform.h"
#include "xadc.h"
#include "xadc_cfg.h"

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

#define XADC_HW_CHANNEL_04 (XSM_CH_AUX_MIN + 0x04)
#define XADC_HW_CHANNEL_12 (XSM_CH_AUX_MIN + 0x0C)


u16 xadc_read_raw(xadc_channel_t channel) {
    u8 _channel;
    //XSysMon_StartAdcConversion(&xSysMon);

    switch (channel) {
        case XADC_CHANNEL_0:
            _channel = XADC_HW_CHANNEL_04;
            break;
        case XADC_CHANNEL_1:
            _channel = XADC_HW_CHANNEL_12;
            break;
        default:
            return 0xffff;
    }

    XSysMon_GetStatus(&xSysMon); /* Clear the old status */
    while ((XSysMon_GetStatus(&xSysMon) & XSM_SR_BUSY_MASK) ==
           XSM_SR_BUSY_MASK);

    return XSysMon_GetAdcData(&xSysMon, _channel);
}

uint16_t xadc_read_mv(xadc_channel_t channel) {
    return (3300 * xadc_read_raw(channel)) >> 15;
}