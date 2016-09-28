
/* C Standard Includes */
#include <string.h>

/* BSP & Xilinx Includes */
#include "xuartlite.h"
#include "xuartlite_l.h"
#include "xparameters.h"

#define WIFI_UART_BUFF_SIZE_POW (10)
#define WIFI_UART_BUFF_SIZE (1<<WIFI_UART_BUFF_SIZE_POW)
#define WIFI_UART_BUFF_MOD(X) (((u32) X ) & (WIFI_UART_BUFF_SIZE - 1))

u8 wifi_uart_rx_buff[WIFI_UART_BUFF_SIZE];
u32 wifi_uart_buff_read_ptr = 0;

XUartLite wifi_uart;

/* Routines */
void wifi_uart_rx_handler(void *CallBackRef, unsigned int EventData) {
	/* Reset Bufefr */
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
    xil_printf(" --- %s ---\r\n", __func__);
	xil_printf("Buffer size: %d\r\n", WIFI_UART_BUFF_SIZE);
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

u32 wifi_uart_read_ERROR(char *buf, u32 maxsize) {
    u32 n = wifi_uart_available();
    u32 count = 0, ptr = wifi_uart_buff_read_ptr;

    if (maxsize < n)
        n = maxsize;

    for (count = 0; ((count < n) && !(
            (wifi_uart_rx_buff[WIFI_UART_BUFF_MOD(ptr + 0)] == 'E') &&
            (wifi_uart_rx_buff[WIFI_UART_BUFF_MOD(ptr + 1)] == 'R') &&
            (wifi_uart_rx_buff[WIFI_UART_BUFF_MOD(ptr + 2)] == 'R') &&
            (wifi_uart_rx_buff[WIFI_UART_BUFF_MOD(ptr + 3)] == 'O') &&
            (wifi_uart_rx_buff[WIFI_UART_BUFF_MOD(ptr + 4)] == 'R') &&
            (wifi_uart_rx_buff[WIFI_UART_BUFF_MOD(ptr + 5)] == '\r'))); count ++){
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

void wifi_uart_task() {
    /* Do nothing */
}
