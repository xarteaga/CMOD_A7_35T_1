
#ifndef _WIFI_ESP8266_H
#define _WIFI_ESP8266_H

/* BSP anx Xilinx Includes */
#include "xparameters.h"

#define WIFI_ESP8266_BASE_ADDR XPAR_AXI_UART_WIFI_BASEADDR

void wifi_esp8266_init ( void );
void wifi_esp8266_task ( void );

#endif /* _WIFI_ESP8266_H */
