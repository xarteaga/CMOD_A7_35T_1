/*
 * vaxi_os.c
 *
 *  Created on: 04/08/2016
 *      Author: xavier
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "xil_types.h"

#include "tinysh.h"
#include "vaxi_os.h"
#include "usb_uart/usb_uart.h"
#include "./remote.h"

#define VAXI_OS_MAX_CMD 128

vaxi_os_fsm_state_t vaxi_os_fsm_state = VAXI_OS_FSM_STATE_IDLE;


void vaxi_os_init() {
	vaxi_os_fsm_state = VAXI_OS_FSM_STATE_IDLE;

	xil_printf("%s ... OK\r\n", __func__);
}

void vaxi_os_put_char(char c) {
	remote_send((char*)&c, 1);
}

void vaxi_os_task() {

}

void vaxi_os_fsm_idle () {

}

void vaxi_os_fsm_lap () {

}

void vaxi_os_fsm_reset () {

}
