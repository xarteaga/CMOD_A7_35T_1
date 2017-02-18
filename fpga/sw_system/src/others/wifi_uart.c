/*
 * wifi_uart.c
 *
 *  Created on: 03/08/2016
 *      Author: xavier
 */

#include <string.h>

#include "xparameters.h"
#include "xuartlite.h"
#include "xuartlite_l.h"
#include "xintc.h"

#include "utils.h"

#define WIFI_UART_BUFF_SIZE_POW (11)
#define WIFI_UART_BUFF_SIZE (1<<WIFI_UART_BUFF_SIZE_POW)
#define WIFI_UART_BUFF_MOD(X) (((u32) X ) & (WIFI_UART_BUFF_SIZE - 1))

u8 wifi_uart_rx_buff[WIFI_UART_BUFF_SIZE];
u32 wifi_uart_buff_write_ptr = 0, wifi_uart_buff_read_ptr = 0;
u32 wifi_uart_read_trial = 0;
u32 wifi_uart_count_loops = 0;

XUartLite wifi_uart;

void wifi_uart_rx_handler(void *CallBackRef, unsigned int EventData) {
	XUartLite_Recv(&wifi_uart, wifi_uart_rx_buff, WIFI_UART_BUFF_SIZE);
	return;
}

void wifi_uart_tx_handler(void *CallBackRef, unsigned int EventData){
	return;
}

void wifi_uart_init () {
	XUartLite_Initialize(&wifi_uart, XPAR_UART_WIFI_DEVICE_ID);
	XUartLite_SetRecvHandler(&wifi_uart, wifi_uart_rx_handler ,&wifi_uart);
	XUartLite_SetSendHandler(&wifi_uart, wifi_uart_tx_handler ,&wifi_uart);
	XUartLite_EnableInterrupt(&wifi_uart);

	XUartLite_Recv(&wifi_uart, wifi_uart_rx_buff, WIFI_UART_BUFF_SIZE);

	wifi_uart_buff_write_ptr = 0;
	wifi_uart_buff_read_ptr = 0;
	wifi_uart_read_trial = 0;
	wifi_uart_count_loops = 0;
	xil_printf("%s ... OK\r\n", __func__);
}

u32 wifi_uart_available() {
	u32 diff;
	wifi_uart_buff_write_ptr = wifi_uart.ReceiveBuffer.NextBytePtr - wifi_uart_rx_buff;
	if (wifi_uart_buff_write_ptr >= wifi_uart_buff_read_ptr)
		diff = wifi_uart_buff_write_ptr - wifi_uart_buff_read_ptr;
	else
		diff = (wifi_uart_buff_write_ptr + WIFI_UART_BUFF_SIZE) - wifi_uart_buff_read_ptr;

	return diff;
}

void wifi_uart_write(u8 *buf, u32 size) {
	XUartLite_Send(&wifi_uart, buf, size);
}

u32 wifi_uart_read(u8 *buf, u32 maxsize) {
	u32 n = wifi_uart_available();
	u32 count = 0;

	if (maxsize < n)
		n = maxsize;

	for (count = 0; count < n; count ++){
		buf[count] = wifi_uart_rx_buff[wifi_uart_buff_read_ptr];
		wifi_uart_buff_read_ptr = WIFI_UART_BUFF_MOD(wifi_uart_buff_read_ptr + 1);
	}

	return n;
}

u32 wifi_uart_read_OK(char *buf, u32 maxsize) {
	u32 n = wifi_uart_available();
	u32 count = 0, ptr = wifi_uart_buff_read_ptr;

	if (maxsize < n)
		n = maxsize;

	for (count = 0; ((count < n) && !(
			(wifi_uart_rx_buff[WIFI_UART_BUFF_MOD(ptr + 0)] == 'O') &&
			(wifi_uart_rx_buff[WIFI_UART_BUFF_MOD(ptr + 1)] == 'K') &&
			(wifi_uart_rx_buff[WIFI_UART_BUFF_MOD(ptr + 2)] == '\r'))); count ++){
			ptr = WIFI_UART_BUFF_MOD(ptr + 1);
	}
	//xil_printf("%d, %d, %s\r\n", n, count, &wifi_uart_rx_buff[wifi_uart_buff_read_ptr]);

	if (count == n) {
		n = 0;
	} else {
		n = wifi_uart_read((u8*)buf, count + 4);
	}

	return n;
}

u32 wifi_uart_task_prev_size = 0;

void wifi_uart_task() {
	u32 size = XUartLite_ReceiveBuffer(&wifi_uart) ;
}
