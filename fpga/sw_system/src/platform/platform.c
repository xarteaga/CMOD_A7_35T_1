
/* Standard C includes */
#include <stdio.h>
#include <stdarg.h>
#include <scheduler.h>

/* BSP & Xilinx Includes */
#include "xparameters.h"
#include "xintc.h"
#include "xuartlite.h"
#include "xspi.h"

/* Project includes */
#include "platform.h"
#include "scheduler.h"

XIntc platform_intc;

//extern XUartLite wifi_uart;
extern XUartLite lcd_uart;
//extern XSpi sd_spi;

void platform_enable_interrupts(void) {
    XIntc *intcp;
    intcp = &platform_intc;

    XIntc_Initialize(intcp, XPAR_INTC_0_DEVICE_ID);

    /* Connect WiFi Module interrupt handler */
    /*XIntc_Connect(intcp, XPAR_INTC_0_UARTLITE_1_VEC_ID,
                  (XInterruptHandler) XUartLite_InterruptHandler,
                  (void *) &wifi_uart);*/

    /* Connect LCD Module interrupt handler */
    XIntc_Connect(intcp, XPAR_INTC_0_UARTLITE_2_VEC_ID,
                  (XInterruptHandler) XUartLite_InterruptHandler,
                  (void *) &lcd_uart);

    /* Connect SD Module Interrupt handler */
    /*XIntc_Connect(intcp, XPAR_INTC_0_SPI_0_VEC_ID,
                  (XInterruptHandler) XSpi_InterruptHandler,
                  (void *) &sd_spi);*/

    /* Connect Fixed Interval Timer interrupt to the scheduler */
    XIntc_Connect(&platform_intc, XPAR_AXI_INTC_0_FIT_TIMER_0_INTERRUPT_INTR,
                  (XInterruptHandler) scheduler_interrupt_handler,
                  NULL);

    /* Start interrupt controller */
    XIntc_Start(intcp, XIN_REAL_MODE);

    /* Start the interrupt controller */
    XIntc_MasterEnable(XPAR_INTC_0_BASEADDR);

    /* Enable Microblaze interrupts and set global interrupt handler */
    microblaze_enable_interrupts();
    microblaze_register_handler((XInterruptHandler) XIntc_InterruptHandler, intcp);

    /* Enable the desired interruptions */
    XIntc_Enable(intcp, XPAR_INTC_0_UARTLITE_1_VEC_ID);
    XIntc_Enable(intcp, XPAR_INTC_0_UARTLITE_2_VEC_ID);
    XIntc_Enable(intcp, XPAR_INTC_0_SPI_0_VEC_ID);
    XIntc_Enable(intcp, XPAR_AXI_INTC_0_FIT_TIMER_0_INTERRUPT_INTR);

    /* Print debug trace */
    LOG("%32s ... OK\r\n", __func__);
}

void init_platform(void) {
    /* Do nothing */
}
