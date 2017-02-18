/*
 * vaxi_os.h
 *
 *  Created on: 04/08/2016
 *      Author: xavier
 */

#ifndef VAXI_OS_H_
#define VAXI_OS_H_

typedef enum {
	VAXI_OS_FSM_STATE_IDLE,
	VAXI_OS_FSM_STATE_WIFI_LAP,
	VAXI_OS_FSM_STATE_WIFI_RESET
} vaxi_os_fsm_state_t;

void vaxi_os_init();

void vaxi_os_task();

#endif /* VAXI_OS_H_ */
