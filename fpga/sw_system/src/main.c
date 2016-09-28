
/* Standard C Includes */
#include <stdio.h>

/* BSP & Xilinx Includes */
#include "xparameters.h"
#include "xgpio.h"

/* Project includes */
#include "platform.h"
#include "wifi_esp8266.h"
#include "scheduler.h"

XGpio Gpio1;

int main() {
    xil_printf("--- CMOD_A7_35T_1 Running ---\r\n");

    /* Init platform */
    init_platform();
    scheduler_init();
    wifi_esp8266_init();

    /* Enable Interrupts */
    platform_enable_interrupts();

    (void) XGpio_Initialize(&Gpio1, XPAR_AXI_GPIO_0_DEVICE_ID);

    while (TRUE) {
        wifi_esp8266_task();

        if (XGpio_DiscreteRead(&Gpio1, 2) != 0){
            xil_printf("> AT\r\n");
            while(XGpio_DiscreteRead(&Gpio1, 2) != 0);
            //wifi_esp8266_send_AT();

        }

    }

    /* End */
    return (int) NULL;
}
