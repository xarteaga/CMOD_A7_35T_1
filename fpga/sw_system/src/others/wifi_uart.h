/*
 * wifi_uart.h
 *
 *  Created on: 03/08/2016
 *      Author: xavier
 */

#ifndef WIFI_UART_H_
#define WIFI_UART_H_

#include "xil_types.h"

void wifi_uart_init();
void wifi_uart_write(u8 *buf, u32 size);
u32 wifi_uart_read_OK(char *buf, u32 maxsize);
void wifi_uart_task();

#endif /* WIFI_UART_H_ */
