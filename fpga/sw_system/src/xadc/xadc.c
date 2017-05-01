//
// Created by vaxi on 1/05/17.
//

/* Standard C Includes */

/* BSP & Xilinx Includes */
#include <scheduler.h>
#include "xparameters.h"
#include "xsysmon.h"

/* Project includes */
#include "platform.h"
#include "xadc.h"

static XSysMon xSysMon;

int xadc_init(void) {
    XSysMon_Config *ConfigPtr;
    int Status;
    XSysMon *SysMonInstPtr = &xSysMon;

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

    PRINT_INIT_OK;
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