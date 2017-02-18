/*
 * remote.h
 *
 *  Created on: 04/08/2016
 *      Author: xavier
 */

#ifndef REMOTE_H_
#define REMOTE_H_

typedef enum {
	OPENAT_SERVER_STATE_LAP,
	OPENAT_SERVER_STATE_IDLE
} openat_server_fsm_state_t;

void remote_init();

void remote_start ();
void remote_task ();

u32 remote_recv(char *buf, u32 size);
void remote_print(const char *format, ...);
void remote_send(char *buf, u32 size);

#endif /* REMOTE_H_ */
