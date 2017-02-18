/*
 * lcd.c
 *
 *  Created on: 31/07/2016
 *      Author: xavier
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "xparameters.h"
#include "xuartlite.h"

#include "server/server.h"
#include "utils.h"

XUartLite lcd_uart;

void lcd_init() {
	XUartLite_Initialize(&lcd_uart, XPAR_UART_LCD_DEVICE_ID);

	xil_printf("%s ... OK\r\n", __func__);
}

lcd_reset() {
	char rst_buf[2] = {124, 'r'};
	lcd_write(rst_buf, 2);
	xil_printf("Reseting LCD\r\n");
}

void lcd_write(u8 *msg, uint8_t line, uint8_t msglen) {
	u8 set_cursor[2] = {0xFE, 0x80};
	u8 line_buffer[16];

	/* Set cursor */
	set_cursor[1] += (line != 2) ? 0 : 64;
	XUartLite_Send(&lcd_uart, set_cursor, sizeof(set_cursor));

	/* Fill buffer */
	msglen = (msglen < 16)?msglen:16;
	memset(line_buffer, ' ', 16);
	memcpy(line_buffer, msg, msglen);

	/* Wait... */
	while(XUartLite_IsSending(&lcd_uart) == TRUE);

	/* Send message */
	XUartLite_Send(&lcd_uart, line_buffer, 16);
}

int lcd_is_writing () {
	return XUartLite_IsSending(&lcd_uart);
}

void lcd_clear_display () {
	u8 clear_display[2] = {0xFE, 0x01};
	XUartLite_Send(&lcd_uart, clear_display, sizeof(clear_display));
}

void lcd_reset_cursor () {
	u8 reset_cursor[2] = {0xFE, 0x80};
	XUartLite_Send(&lcd_uart, reset_cursor, sizeof(reset_cursor));
}

void *lcd_callback(request_t *req, response_t *res){
	char *path = req->url;
	char *argv[10];
	int argn = path2args(path, argv), line;

	if (argn == 4 && strcmp(argv[1], "line") == 0) {
		//lcd_clear_display();
		line = atoi(argv[2]);
		if (line > 0) {
			lcd_write(argv[3], line, strlen(argv[3]));
		} else {
			res->code = RES_BAD_REQUEST;
			res->content = "Bad number of arguments. Usage: /lcd/$operation [reset,line]$/$line [1, 2]$/$message$\r\n";
		}

	} else if (argn == 2 && strcmp(argv[1], "reset") == 0) {
		lcd_reset();
	} else {
		res->code = RES_BAD_REQUEST;
		res->content = "Bad number of arguments. Usage: /lcd/$operation [reset,line]$/$line [1, 2]$/$message$\r\n";
	}

	return 0;
}
