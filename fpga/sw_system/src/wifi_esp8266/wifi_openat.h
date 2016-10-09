
#ifndef _WIFI_OPENAT_C_H
#define _WIFI_OPENAT_C_H

/* Standard C includes */
#include <inttypes.h>

/* Project includes */
#include <buffer.h>

/* Data-types */
typedef enum {
    WIFI_OPENAT_STATE_UNDEFINED = 0,
    WIFI_OPENAT_STATE_IDLE,
    WIFI_OPENAT_STATE_DONE_OK,
    WIFI_OPENAT_STATE_DONE_ERROR,
    WIFI_OPENAT_STATE_WAIT_FOR_DATA,
    WIFI_OPENAT_STATE_RECEIVING,
    WIFI_OPENAT_STATE_BUSY
} t_wifi_openat_state;

typedef enum {
    WIFI_OPENAT_RETURN_OK = 0,
    WIFI_OPENAT_RETURN_NOK = -1
} t_wifi_openat_return;

/* FSM Interfaces */
t_wifi_openat_state wifi_openat_get_state(void);
t_wifi_openat_return wifi_openat_send_cmd (uint8_t *cmd);
t_wifi_openat_return wifi_openat_send_data(uint8_t *cmd, uint8_t *data, size_t len);
void wifi_openat_set_tcp_recv_buffer(t_buffer *_tcp_recv_buffer);
size_t wifi_openat_read (uint8_t *buf, size_t maxlen);
void wifi_openat_set_tcp_recv_buffer(t_buffer *_tcp_recv_buffer);

/* FSM init */
void wifi_openat_init (void);

/* FSM main task */
void wifi_openat_task (void);

#endif //_WIFI_OPENAT_C_H
