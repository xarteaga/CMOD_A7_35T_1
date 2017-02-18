
/* Standard C */
#include <inttypes.h>
#include <string.h>

/* BSP and Xilinx Includes */
#include <xparameters.h>
#include <xspi.h>
#include <xgpio.h>

/* Project Includes */
#include "sd_spi.h"

/* Definitions */
#define SD_SPI_DEVICE_ID        XPAR_AXI_SPI_SD_DEVICE_ID
#define SD_GPIO_DEVICE_ID    XPAR_AXI_GPIO_SD_CS_DEVICE_ID
#define SD_SPI_BUFFER_TX_SIZE   256
#define SD_SPI_BUFFER_RX_SIZE   256

/* Global Variables */
XSpi sd_spi;
XGpio sd_gpio;

uint8_t sd_spi_buffer_tx [SD_SPI_BUFFER_TX_SIZE];
uint8_t sd_spi_buffer_rx [SD_SPI_BUFFER_RX_SIZE];

void* sd_spi_handler (void *CallBackRef, u32 StatusEvent, uint32_t ByteCount) {
    uint32_t i;

    XGpio_DiscreteWrite(&sd_gpio, 1, 1);

    switch(StatusEvent) {
#define PRINT_STATUS(s) case s: xil_printf("[%s] New status event: %s (%d, %d)\r\n", __FUNCTION__, #s, s, ByteCount); break
        PRINT_STATUS(XST_SPI_TRANSFER_DONE);
        PRINT_STATUS(XST_SPI_TRANSMIT_UNDERRUN);
        PRINT_STATUS(XST_SPI_RECEIVE_OVERRUN);
        PRINT_STATUS(XST_SPI_NO_SLAVE);
        PRINT_STATUS(XST_SPI_TOO_MANY_SLAVES);
        PRINT_STATUS(XST_SPI_NOT_MASTER);
        PRINT_STATUS(XST_SPI_SLAVE_ONLY);
        PRINT_STATUS(XST_SPI_SLAVE_MODE_FAULT);
        PRINT_STATUS(XST_SPI_SLAVE_MODE);
        PRINT_STATUS(XST_SPI_RECEIVE_NOT_EMPTY);
        PRINT_STATUS(XST_SPI_COMMAND_ERROR);
        default:
            xil_printf("[%s] New status event: %s (%d, %d)\r\n", __FUNCTION__, "UNDEFINED", StatusEvent, ByteCount);
#undef PRINT_STATUS
    }

    if (StatusEvent == XST_SPI_TRANSFER_DONE) {
        xil_printf("Tx:\r\n");
        for ( i = 0; i < ByteCount; i++) {
            xil_printf("%02X ", sd_spi_buffer_tx[i]);
            if ((i & 0x0F) == 0x0F) {
                xil_printf("\r\n");
            }
        }
    }

    if (StatusEvent == XST_SPI_TRANSFER_DONE) {
        xil_printf("Rx:\r\n");
        for ( i = 0; i < ByteCount; i++) {
            xil_printf("%02X ", sd_spi_buffer_rx[i]);
            if ((i & 0x0F) == 0x0F) {
                xil_printf("\r\n");
            }
        }
    }

    return NULL;
}

void sd_spi_send_command (t_sd_spi_cmd cmd){
    uint32_t i;
    XStatus status;

    (void) memset(sd_spi_buffer_tx, 0xFF, SD_SPI_BUFFER_TX_SIZE);
    (void) memset(sd_spi_buffer_rx, 0x66, SD_SPI_BUFFER_TX_SIZE);

    sd_spi_buffer_tx[0] = 0x40 | ((uint8_t) cmd);
    xil_printf("[%s]\r\n", __FUNCTION__);

    /*xil_printf("Tx:\r\n");
    for ( i = 0; i < SD_SPI_BUFFER_TX_SIZE; i++) {
        xil_printf("%02X ", sd_spi_buffer_tx[i]);
        if ((i & 0x0F) == 0x0F) {
            xil_printf("\r\n");
        }
    }*/

    XGpio_DiscreteWrite(&sd_gpio, 1, 1);
    status = XSpi_Transfer(&sd_spi, sd_spi_buffer_tx, sd_spi_buffer_rx, SD_SPI_BUFFER_TX_SIZE);
    if (status != XST_SUCCESS) {
        xil_printf("[%s] Error ocurred when XSpi_Transfer %d\r\n", __FUNCTION__, (int) status);
    }

    xil_printf("[%s]\r\n", __FUNCTION__);

}

void sd_spi_init(void) {
    XStatus status;

    /* Initialize SPI controller */
    status = XSpi_Initialize(&sd_spi,SD_SPI_DEVICE_ID);
    if (status != XST_SUCCESS) {
        xil_printf("[%s] Error ocurred when XSpi_CfgInitialize %d\r\n", __FUNCTION__, (int) status);
    }

    XSpi_SetStatusHandler(&sd_spi, &sd_spi, (XSpi_StatusHandler) sd_spi_handler);

    status = XSpi_SetOptions(&sd_spi, XSP_MASTER_OPTION | XSP_MANUAL_SSELECT_OPTION);
    if (status != XST_SUCCESS) {
        xil_printf("[%s] Error ocurred when XSpi_SetOptions %d\r\n", __FUNCTION__, (int) status);
    }
    sd_spi.SlaveSelectReg = ~((uint32_t)0x01);

    status = XSpi_Start(&sd_spi);
    if (status != XST_SUCCESS) {
        xil_printf("[%s] Error ocurred when XSpi_Start %d\r\n", __FUNCTION__, (int) status);
    }

    /* Initialize GPIO controller */
    XGpio_Initialize(&sd_gpio, SD_GPIO_DEVICE_ID);
    XGpio_DiscreteWrite(&sd_gpio, 1, 1);

    xil_printf("%32s ... OK\r\n", __func__);
}