
/* C Standard Includes */
#include <string.h>

/* BSP & Xilinx Includes */
#include "xuartlite.h"
#include "xuartlite_l.h"
#include "xparameters.h"

#define WIFI_UART_BUFF_SIZE_POW (12)
#define WIFI_UART_BUFF_SIZE (1<<WIFI_UART_BUFF_SIZE_POW)
#define WIFI_UART_BUFF_MOD(X) (((u32) X ) & (WIFI_UART_BUFF_SIZE - 1))

u8 wifi_uart_rx_buff[WIFI_UART_BUFF_SIZE];
u32 wifi_uart_buff_read_ptr = 0;

u8 wifi_uart_tx_buff[WIFI_UART_BUFF_SIZE];


XUartLite wifi_uart;

/* Routines */
void wifi_uart_rx_handler(void *CallBackRef, unsigned int EventData) {
	/* Reset Buffer */
    XUartLite_Recv(&wifi_uart, wifi_uart_rx_buff, WIFI_UART_BUFF_SIZE);

    return;
}

void wifi_uart_tx_handler(void *CallBackRef, unsigned int EventData){
    /* Do nothing */
	return;
}

void wifi_uart_init (void) {
	XUartLite_Initialize(&wifi_uart, XPAR_AXI_UART_WIFI_DEVICE_ID);
	XUartLite_SetRecvHandler(&wifi_uart, wifi_uart_rx_handler ,&wifi_uart);
	XUartLite_SetSendHandler(&wifi_uart, wifi_uart_tx_handler ,&wifi_uart);
	XUartLite_EnableInterrupt(&wifi_uart);

	XUartLite_Recv(&wifi_uart, wifi_uart_rx_buff, WIFI_UART_BUFF_SIZE);

	wifi_uart_buff_read_ptr = 0;
    xil_printf("%32s ... OK\r\n", __func__);
}

u32 wifi_uart_available() {
	u32 diff;
	u32 wifi_uart_buff_write_ptr = (u32)wifi_uart.ReceiveBuffer.NextBytePtr - (u32)wifi_uart_rx_buff;
	if (wifi_uart_buff_write_ptr >= wifi_uart_buff_read_ptr) {
		diff = wifi_uart_buff_write_ptr - wifi_uart_buff_read_ptr;
	} else {
		diff = (u32) ((wifi_uart_buff_write_ptr + WIFI_UART_BUFF_SIZE) - wifi_uart_buff_read_ptr);
	}

	return diff;
}

void wifi_uart_write(u8 *buf, u32 size) {
	memcpy(wifi_uart_tx_buff, buf, size);
	XUartLite_Send(&wifi_uart, wifi_uart_tx_buff, size);
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

size_t wifi_uart_read_key(uint8_t *buf, size_t maxsize, uint8_t *key) {
	size_t keylen = strlen((char*)key);
	u32 n = wifi_uart_available();
	u32 count = 0, ptr = wifi_uart_buff_read_ptr;
	uint8_t match = FALSE;
	u32 k;

	if (maxsize < n)
		n = maxsize;

	for (count = 0; ((count < n) && (match != TRUE)); count ++){
		/* Recover pointer */
		ptr = WIFI_UART_BUFF_MOD(wifi_uart_buff_read_ptr + count);

		/* Check key match */
		for (k = 0; (k < keylen) && (wifi_uart_rx_buff[ptr] == key[k]); k++) {
			ptr = WIFI_UART_BUFF_MOD(ptr + 1);
		}

		/* If for loop reaches its end ... */
		if ( k == keylen ) {
			/* It is a match */
			match = TRUE;
		}
	}

	if (match == TRUE) {
		n = wifi_uart_read(buf, count + keylen - 1);
	} else {
		n = 0;
	}

	return n;
}

void wifi_uart_task() {
    /* Do nothing */
}
