/*
 * wifi.c
 *
 *  Created on: 31/07/2016
 *      Author: xavier
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "wifi.h"
#include "tinysh.h"
#include "wifi_uart.h"

#include "utils/wifi_utils.h"

wifi_fsm_state_t wifi_fsm_state;
wifi_command_t wifi_command;

#define WIFI_BUFF_MAX_SIZE 1024
char wifi_buff[WIFI_BUFF_MAX_SIZE];

wifi_ap_t wifi_ap_list [WIFI_AP_MAX_LIST];

/* FSM Functions */
void wifi_fsm_idle ();
void wifi_fsm_cwlap ();
void wifi_fsm_reset ();

/* Other functions */
void wifi_request_lap (int argc, char **argv);
void wifi_request_reset (int argc, char **argv);

void wifi_init () {
	wifi_fsm_state = WIFI_FSM_IDLE;
	wifi_command = WIFI_COMMAND_NONE;

	xil_printf("%s ... OK\r\n", __func__);
}

void wifi_task() {
	switch(wifi_fsm_state){
	case WIFI_FSM_IDLE:
		wifi_fsm_idle ();
		break;
	case WIFI_FSM_CWLAP:
		wifi_fsm_cwlap();
		break;
	case WIFI_FSM_RESET:
		wifi_fsm_reset();
		break;
	default:
		wifi_fsm_state = WIFI_FSM_IDLE;
	}
}

void wifi_request_lap (int argc, char **argv) {
	wifi_command = WIFI_COMMAND_LIST_ACCESS_POINTS;
}

void wifi_request_reset (int argc, char **argv) {
	wifi_command = WIFI_COMMAND_RESET;
}

wifi_ap_t* wifi_get_ap_list(){
	return wifi_ap_list;
}

wifi_fsm_state_t wifi_get_fsm_state(){
	return wifi_fsm_state;
}

void wifi_fsm_idle () {
	u8 cmd_list_access_point [] = "AT+CWLAP\r\n";
	u8 cmd_reset [] = "AT+RST\r\n";

	if (wifi_command == WIFI_COMMAND_LIST_ACCESS_POINTS) {
		// Send UART OpenAT Command
		wifi_uart_write(cmd_list_access_point, sizeof(cmd_list_access_point));

		// Clean command
		wifi_command = WIFI_COMMAND_NONE;

		// Change state
		wifi_fsm_state = WIFI_FSM_CWLAP;

	}else if (wifi_command == WIFI_COMMAND_RESET) {
		// Send UART OpenAT Command
		wifi_uart_write(cmd_reset, sizeof(cmd_reset));

		// Clean command
		wifi_command = WIFI_COMMAND_NONE;

		// Change state
		wifi_fsm_state = WIFI_FSM_RESET;

	} else {
		wifi_fsm_state = WIFI_FSM_IDLE;
	}
}

void wifi_fsm_cwlap () {
	u32 n = wifi_uart_read_OK(wifi_buff, WIFI_BUFF_MAX_SIZE);
	if (n > 0) {
		wifi_buff[n] = 0;
		xil_printf("[[[%s]]]\r\n", wifi_buff);

		n = wifi_utils_parse_cwlap(wifi_buff, wifi_ap_list);
		wifi_ap_list[n].ecn = WIFI_AP_ECN_EMPTY;

		xil_printf("[wifi_fsm_state] WIFI_FSM_CWLAP -> WIFI_FSM_IDLE \r\n");

		wifi_fsm_state = WIFI_FSM_IDLE;
	} else {
		wifi_fsm_state = WIFI_FSM_CWLAP;
	}
}

void wifi_fsm_reset () {
	u32 n = wifi_uart_read_OK(wifi_buff, WIFI_BUFF_MAX_SIZE);
	wifi_buff[n] = 0;
	xil_printf("[[[%s]]]\r\n", wifi_buff);
	if (n > 0) {

		wifi_fsm_state = WIFI_FSM_IDLE;
	} else {
		wifi_fsm_state = WIFI_FSM_RESET;
	}
}
