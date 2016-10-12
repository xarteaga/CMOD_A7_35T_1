
/* Standard C includes */
#include <inttypes.h>

/* BSP and Xilinx Includes */
#include "xparameters.h"
#include "xgpio.h"

/* Project includes */
#include "wifi_gpio.h"

#define WIFI_ENABLE_GPIO_ID     XPAR_AXI_GPIO_WIFI_EN_DEVICE_ID
#define WIFI_ENABLE_GPIO_CHAN   1

void wifi_gpio_enable (void) {
    XGpio wifi_gpio;
    (void) XGpio_Initialize(&wifi_gpio, WIFI_ENABLE_GPIO_ID);
    XGpio_DiscreteWrite(&wifi_gpio, WIFI_ENABLE_GPIO_CHAN, (uint32_t) 0x01);
}

