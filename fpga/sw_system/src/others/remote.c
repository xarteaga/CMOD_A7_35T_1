/*
 * remote.c
 *
 *  Created on: 04/08/2016
 *      Author: xavier
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#include "lwip/err.h"
#include "lwip/tcp.h"

#include "remote.h"
#include "usb_uart/usb_uart.h"

#define OPENAT_SERVER_PORT 22
#define MAX_BUFFER_SIZE 1024

struct tcp_pcb *client = NULL;
openat_server_fsm_state_t openat_server_fsm_state;


char remote_buff_recv [MAX_BUFFER_SIZE];
char remote_buff_send [MAX_BUFFER_SIZE];

u32 remote_buff_recv_write_ptr, remote_buff_recv_read_ptr;
u32 remote_buff_send_write_ptr, remote_buff_send_read_ptr;

u32 remote_available() {
	u32 n;
	if (remote_buff_recv_write_ptr >= remote_buff_recv_read_ptr) {
		n = remote_buff_recv_write_ptr - remote_buff_recv_read_ptr;
	} else {
		n = (remote_buff_recv_write_ptr + MAX_BUFFER_SIZE) - remote_buff_recv_read_ptr;
	}
	return n;
}

void remote_print(const char *format, ...){
	int size;
	char buffer[515];
	va_list args;

	/* Format string */
	va_start (args, format);
	size = vsprintf(buffer, format, args);
	va_end (args);

	/* Send */
	remote_send(buffer, size);
}

void remote_send(char *buf, u32 size) {
	u32 n;
	for (n = 0; n < size; n++) {
		remote_buff_send[remote_buff_send_write_ptr] = buf[n];
		remote_buff_send_write_ptr = (remote_buff_send_write_ptr + 1)%MAX_BUFFER_SIZE;
	}
}

u32 remote_recv(char *buf, u32 size) {
	u32 n = remote_available();

	if (size > n)
		size = n;

	for (n = 0; n < size; n++) {
		buf[n] = remote_buff_recv[remote_buff_recv_read_ptr];
		remote_buff_recv_read_ptr = (remote_buff_recv_read_ptr + 1)%MAX_BUFFER_SIZE;
	}

	return n;
}

void openat_server_idle();
void openat_server_lap();

err_t remote_recv_callback(void *arg, struct tcp_pcb *tpcb,
                               struct pbuf *p, err_t err)
{
	u32 n;
	char strbuf[MAX_BUFFER_SIZE];

	/* do not read the packet if we are not in ESTABLISHED state */
	if (!p) {
		tcp_close(tpcb);
		tcp_recv(tpcb, NULL);
		return ERR_OK;
	}

	/* indicate that the packet has been received */
	tcp_recved(tpcb, p->len);

	memcpy(strbuf, p->payload, p->len);
	strbuf[p->len] = 0;

	if (strcmp(strbuf, "exit\r\n") == 0){
		/* Send bye */
		tcp_write(tpcb, "Bye ;)\r\n", 8, 1);


		/* Close connection */
		tcp_close(tpcb);
		tcp_recv(tpcb, NULL);
		client = NULL;
	} else {
		for (n = 0; n < p->len; n++) {
			remote_buff_recv[remote_buff_recv_write_ptr] = ((char*)p->payload)[n];
			remote_buff_recv_write_ptr = (remote_buff_recv_write_ptr + 1)%MAX_BUFFER_SIZE;
		}

		/* free the received pbuf */
		pbuf_free(p);
	}

	return ERR_OK;
}

err_t remote_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	static int connection = 1;

		if (client == NULL) {
			client = newpcb;

		/* set the receive callback for this connection */
		tcp_recv(newpcb, remote_recv_callback);

		/* just use an integer number indicating the connection id as the
		   callback argument */
		tcp_arg(newpcb, (void*)connection);

		/* increment for subsequent accepted connections */
		connection++;

		/* Welcome to VaXiOS */
		tcp_write(newpcb, "--- [ Welcome to VaXiOS ] ---\r\n", 32, 1);

	}else {
		tcp_close(newpcb);
	}


	return ERR_OK;
}

void remote_init () {
	struct tcp_pcb *pcb;
	err_t err;

	/* create new TCP PCB structure */
	pcb = tcp_new();
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\n\r");
		return;
	}

	/* bind to specified @port */
	err = tcp_bind(pcb, IP_ADDR_ANY, OPENAT_SERVER_PORT);
	if (err != ERR_OK) {
		xil_printf("Unable to bind to port %d: err = %d\n\r", OPENAT_SERVER_PORT, err);
		return;
	}

	/* we do not need any arguments to callback functions */
	tcp_arg(pcb, NULL);

	/* listen for connections */
	pcb = tcp_listen(pcb);
	if (!pcb) {
		xil_printf("Out of memory while tcp_listen\n\r");
		return;
	}

	/* specify callback to use for incoming connections */
	tcp_accept(pcb, remote_accept_callback);

	openat_server_fsm_state = OPENAT_SERVER_STATE_IDLE;
	xil_printf("%s ... OK\r\n", __func__);
	return;
}

void remote_task () {
	u8 buf[MAX_BUFFER_SIZE + 1];
	u32 n, size;
	char c;


	/* Get send buffer enqueued data size */
	if (remote_buff_send_write_ptr >= remote_buff_send_read_ptr) {
		size = remote_buff_send_write_ptr - remote_buff_send_read_ptr;
	} else {
		size = (remote_buff_send_write_ptr + MAX_BUFFER_SIZE) - remote_buff_send_read_ptr;
	}

	if (size > MAX_BUFFER_SIZE)
		size = MAX_BUFFER_SIZE;

	for (n = 0; n < size; n++){
		buf[n] = remote_buff_send[remote_buff_send_read_ptr];
		usb_uart_write(&buf[n], 1);
		remote_buff_send_read_ptr = (remote_buff_send_read_ptr + 1)%MAX_BUFFER_SIZE;
	}

	/* If there is a client then transmit over the socket */
	if (client != NULL) {
		tcp_write(client, buf, size, 1);
	}

	while(usb_uart_read(&c, 1)>0) {
		remote_buff_recv[remote_buff_recv_write_ptr] = c;
		remote_buff_recv_write_ptr = (remote_buff_recv_write_ptr + 1)%MAX_BUFFER_SIZE;
	}

}

