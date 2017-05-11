
/* Standard C Includes */
#include <stdio.h>

/* BSP & Xilinx Includes */

/* Project includes */
#include "platform.h"
#include "wifi_esp8266.h"
#include "scheduler.h"
//#include "http_client.h"
//#include "sd_spi.h"
#include <xadc.h>
#include <supercap.h>
#include <dinouts.h>
#include <leds.h>
#include <buttons.h>
#include <irproximity.h>
#include <actuators.h>
#include <rtc.h>
#include <happypills.h>
#include <lcd.h>

int main(void) {
    xil_printf("\r\n--- CMOD_A7_35T_1 Running %s %s ---\r\n\r\n", __DATE__, __TIME__);

    /* Init platform */
    scheduler_init();
    xadc_init();
    supercap_init();
    //wifi_esp8266_init();
    //http_client_init ();
    //sd_spi_init();
    dinouts_init();
    leds_init();
    buttons_init();
    irproximity_init();
    actuators_init();
    rtc_init();
    lcd_init();

    happypills_init();

    /* Enable Interrupts (after all initialization calls) */
    platform_enable_interrupts();
    init_platform();

    //sd_spi_send_command (SD_SPI_CMD0_GO_IDLE_STATE);

    while (TRUE) {
        //wifi_esp8266_task();
        //http_client_task();

        /*if (XGpio_DiscreteRead(&Gpio1, 2) != 0){
            xil_printf("> SPI SD CMD0\r\n");
            while(XGpio_DiscreteRead(&Gpio1, 2) != 0)
                sd_spi_send_command(SD_SPI_CMD0_GO_IDLE_STATE);

        }*/
    }

    /* End */
    return (int) NULL;
}
