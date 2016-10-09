
/* Standard C includes */

/* BSP & Xilinx Includes */
#include "xintc.h"
#include "xuartlite.h"
#include "xparameters.h"

/* Project includes */
#include "platform.h"
#include "scheduler.h"

XIntc platform_intc;

extern XUartLite wifi_uart;

void platform_enable_interrupts(void) {
    XIntc *intcp;
    intcp = &platform_intc;

    XIntc_Initialize(intcp, XPAR_INTC_0_DEVICE_ID);

    XIntc_Connect(intcp, XPAR_INTC_0_UARTLITE_1_VEC_ID,
                  (XInterruptHandler)XUartLite_InterruptHandler,
                  (void *)&wifi_uart);

    XIntc_Connect(&platform_intc, XPAR_AXI_INTC_0_FIT_TIMER_0_INTERRUPT_INTR,
                  (XInterruptHandler)scheduler_interrupt_handler,
                  NULL);

    XIntc_Start(intcp, XIN_REAL_MODE);

    /* Start the interrupt controller */
    XIntc_MasterEnable(XPAR_INTC_0_BASEADDR);

    microblaze_enable_interrupts();
    microblaze_register_handler((XInterruptHandler)XIntc_InterruptHandler, intcp);

    XIntc_Enable(intcp, XPAR_INTC_0_UARTLITE_1_VEC_ID);
    XIntc_Enable(intcp, XPAR_AXI_INTC_0_FIT_TIMER_0_INTERRUPT_INTR);

    xil_printf("%32s ... OK\r\n", __func__);
}

void init_platform(void)
{

}
