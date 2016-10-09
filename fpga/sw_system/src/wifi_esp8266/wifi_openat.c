/* C Standard includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Project includes */
#include "wifi_openat.h"
#include "wifi_uart.h"
#include "scheduler.h"
#include "buffer.h"
#include "wifi_cfg.h"

/* Definitions */
#define WIFI_OPENAT_SEND_BUFFER_SIZE 512
#define WIFI_OPENAT_RECV_BUFFER_SIZE 512
#define WIFI_OPENAT_CMD_TIMEOUT      10000

/* Function prototypes */
static scheduler_callback wifi_openat_task_timer(u32 elapsed);

/* Variables */
static t_wifi_openat_state wifi_openat_state = WIFI_OPENAT_STATE_UNDEFINED;
static scheduler_entry_t wifi_openat_task_timer_entry = {0, 10, wifi_openat_task_timer};
static u32 wifi_openat_timer = 0;

static uint8_t wifi_openat_recv_buffer[WIFI_OPENAT_RECV_BUFFER_SIZE];
static size_t wifi_openat_recv_buffer_size;

static u8 wifi_openat_send_buffer[WIFI_OPENAT_SEND_BUFFER_SIZE];
static size_t wifi_openat_send_buffer_size;

static size_t tcp_recv_nbytes;
static t_buffer *tcp_recv_buffer;

/*
 * FSM Interface functions
 */
t_wifi_openat_state wifi_openat_get_state(void) {
    /* Return current state */
    return wifi_openat_state;
}

t_wifi_openat_return wifi_openat_send_cmd(uint8_t *cmd) {
    t_wifi_openat_return ret;

    /* Check FSM state */
    if (wifi_openat_state == WIFI_OPENAT_STATE_IDLE) {

        /* Send command */
        wifi_uart_write(cmd, strlen((char *) cmd));

        /* Go to busy state */
        wifi_openat_state = WIFI_OPENAT_STATE_BUSY;

        /* set return */
        ret = WIFI_OPENAT_RETURN_OK;
    } else {
        ret = WIFI_OPENAT_RETURN_NOK;
    }

    return ret;
}

t_wifi_openat_return wifi_openat_send_data(uint8_t *cmd, uint8_t *data, size_t len) {
    t_wifi_openat_return ret;

    /* Check FSM state */
    if (wifi_openat_state == WIFI_OPENAT_STATE_IDLE) {
        /* Copy data */
        memcpy(wifi_openat_send_buffer, data, len);
        wifi_openat_send_buffer_size = len;

        /* Send command */
        wifi_uart_write(cmd, strlen((char *) cmd));

        /* Go to busy state */
        wifi_openat_state = WIFI_OPENAT_STATE_WAIT_FOR_DATA;

        /* set return */
        ret = WIFI_OPENAT_RETURN_OK;
    } else {
        ret = WIFI_OPENAT_RETURN_NOK;
    }

    return ret;
}

void wifi_openat_set_tcp_recv_buffer(t_buffer *_tcp_recv_buffer) {
    tcp_recv_buffer = _tcp_recv_buffer;
}

size_t wifi_openat_read(u8 *buf, size_t maxlen) {
    size_t n;
    if (wifi_openat_recv_buffer_size > maxlen) {
        n = maxlen;
    } else {
        n = wifi_openat_recv_buffer_size;
    }

    (void) memcpy(buf, wifi_openat_recv_buffer, n);

    wifi_openat_recv_buffer_size = 0;
    wifi_openat_state = WIFI_OPENAT_STATE_IDLE;

    return n;
}

/* FSM state Routines */
static scheduler_callback wifi_openat_task_timer(u32 elapsed) {
    wifi_openat_timer += elapsed;

    return NULL;
}

static void wifi_openat_state_undefined(void) {
    uint8_t cmd[] = "AT+RST\r\n";
    wifi_uart_write(cmd, sizeof(cmd));

    /* Go to IDLE */
    wifi_openat_state = WIFI_OPENAT_STATE_IDLE;
}

static void wifi_openat_state_idle(void) {
    size_t n_recv = wifi_uart_read_key(wifi_openat_recv_buffer, WIFI_OPENAT_RECV_BUFFER_SIZE - 1,
                                       (uint8_t *) "+IPD,4,");
    int32_t nbytes = -1;

    if (n_recv > 0) {
        wifi_openat_recv_buffer[n_recv] = 0;

#if OPENAT_DEBUG_TRACES == 1
        xil_printf("[%s] Data received\r\n%s\r\n", __FUNCTION__, (char *) wifi_openat_recv_buffer);
#endif /* OPENAT_DEBUG_TRACES == 1 */

        /* Parse nbytes */
        n_recv = wifi_uart_read_key(wifi_openat_recv_buffer, WIFI_OPENAT_RECV_BUFFER_SIZE - 1, (uint8_t *) ":");
        wifi_openat_recv_buffer[n_recv - 1] = 0;
        nbytes = atoi((char *) wifi_openat_recv_buffer);
    }

    if (nbytes > 0) {
        /* Set buffer size */
        tcp_recv_nbytes = (size_t) nbytes;

#if OPENAT_DEBUG_TRACES == 1
        xil_printf("[%s] Receiving %d bytes\r\n", __FUNCTION__, (char *) tcp_recv_nbytes);
#endif /* OPENAT_DEBUG_TRACES == 1 */

        wifi_openat_state = WIFI_OPENAT_STATE_RECEIVING;
    } else {
        /* Do nothing, keep being in IDLE */
        wifi_openat_state = WIFI_OPENAT_STATE_IDLE;
    }
}

static void wifi_openat_state_busy(void) {
    u32 n_ok, n_err = 0;

    /* Read response */
    n_ok = wifi_uart_read_key(wifi_openat_recv_buffer, WIFI_OPENAT_RECV_BUFFER_SIZE - 1, (uint8_t *) "OK\r\n");
    if (n_ok == 0) {
        n_err = wifi_uart_read_key(wifi_openat_recv_buffer, WIFI_OPENAT_RECV_BUFFER_SIZE - 1, (uint8_t *) "ERROR\r\n");
    }

    /* Take decision */
    if (n_ok > 0) {
        /* Set last byte to zero */
        wifi_openat_recv_buffer[n_ok] = 0;
        wifi_openat_recv_buffer_size = n_ok;

        /* Change state */
        wifi_openat_state = WIFI_OPENAT_STATE_DONE_OK;
    } else if (n_err > 0) {
        /* Set last byte to zero */
        wifi_openat_recv_buffer[n_err] = 0;
        wifi_openat_recv_buffer_size = n_err;

        /* Change state */
        wifi_openat_state = WIFI_OPENAT_STATE_DONE_ERROR;
    } else if (wifi_openat_timer > WIFI_OPENAT_CMD_TIMEOUT) {
#if OPENAT_DEBUG_TRACES == 1
        xil_printf("[%s] Timeout \r\n", __FUNCTION__);
#endif /* OPENAT_DEBUG_TRACES == 1 */

        /* Change state */
        wifi_openat_state = WIFI_OPENAT_STATE_DONE_ERROR;
    } else {
        /* Do nothing, keep being in BUSY */
        wifi_openat_state = WIFI_OPENAT_STATE_BUSY;
    }
}

static void wifi_openat_state_done_ok(void) {
    /* Do nothing, keep being in DONE_OK */
    wifi_openat_state = WIFI_OPENAT_STATE_DONE_OK;
}

static void wifi_openat_state_done_error(void) {
    /* Do nothing, keep being in DONE_ERROR */
    wifi_openat_state = WIFI_OPENAT_STATE_IDLE;
}

static void wifi_openat_state_wait_for_data(void) {
    size_t n_ok = wifi_uart_read_key(wifi_openat_recv_buffer, WIFI_OPENAT_RECV_BUFFER_SIZE, (uint8_t *) ">");

    if (n_ok > 0) {
        /* Print trace */
#if OPENAT_DEBUG_TRACES == 1
        xil_printf("[%s] Ready for sending data:\r\n%s\r\n", __FUNCTION__, wifi_openat_send_buffer);
#endif /* OPENAT_DEBUG_TRACES == 1 */

        /* Send stored data */
        wifi_uart_write(wifi_openat_send_buffer, wifi_openat_send_buffer_size);

        /* Do nothing, keep being in DONE_ERROR */
        wifi_openat_state = WIFI_OPENAT_STATE_BUSY;

    } else {
        /* Do nothing, keep being in DONE_ERROR */
        wifi_openat_state = WIFI_OPENAT_STATE_WAIT_FOR_DATA;
    }
}

static void wifi_openat_receiving(void) {
    uint8_t byte;
    size_t n = 0;
    uint32_t i;
    size_t n_uart_available = wifi_uart_available();
    size_t n_buffer_free = tcp_recv_buffer->size - buffer_available(tcp_recv_buffer);

    /* Limit available bytes to the current received packet */
    if (n_uart_available > tcp_recv_nbytes) {
        n_uart_available = tcp_recv_nbytes;
    }

    /* Check buffer availability */
    if (n_uart_available > n_buffer_free) {
        n = n_buffer_free;
    } else {
        n = n_uart_available;
    }

    /* Write buffer */
    for (i = 0; i < n; i++) {
        (void) wifi_uart_read(&byte, 1);
        (void) buffer_write_byte(tcp_recv_buffer, &byte);
    }

    /* Subtract number of received bytes */
    tcp_recv_nbytes -= n;

    /* Check if all the bytes have been received */
    if (tcp_recv_nbytes == 0) {
#if OPENAT_DEBUG_TRACES == 1
        xil_printf("[%s] All Bytes have been received (%d), going back to idle...\r\n", __FUNCTION__,
                   buffer_available(tcp_recv_buffer));
#endif /* OPENAT_DEBUG_TRACES == 1 */

        /* Go to idle */
        wifi_openat_state = WIFI_OPENAT_STATE_IDLE;
    } else {
        /* Keep the same state */
        wifi_openat_state = WIFI_OPENAT_STATE_RECEIVING;
    }
}

/*
 * FSM Public routines
 */
void wifi_openat_init(void) {
    /* Init UART Wifi Driver */
    wifi_uart_init();

    /* Add task entry to the scheduler */
    scheduler_add_entry(&wifi_openat_task_timer_entry);

    /* Set all global variables to zero */
    wifi_openat_recv_buffer_size = 0;
    wifi_openat_send_buffer_size = 0;
    tcp_recv_nbytes = 0;
    tcp_recv_buffer = NULL;

    /* Initialise FSM state */
    wifi_openat_state = WIFI_OPENAT_STATE_UNDEFINED;

    /* Run FSM task */
    wifi_openat_task();

    xil_printf("%32s ... OK\r\n", __func__);
}

void wifi_openat_task(void) {
    /* Copy current state */
    t_wifi_openat_state wifi_openat_state_prev = wifi_openat_state;

    /* Run UART task */
    wifi_uart_task();

    /* Depending on the state run one or other state */
    switch (wifi_openat_state) {
        case WIFI_OPENAT_STATE_UNDEFINED:
            wifi_openat_state_undefined();
            break;
        case WIFI_OPENAT_STATE_IDLE:
            wifi_openat_state_idle();
            break;
        case WIFI_OPENAT_STATE_BUSY:
            wifi_openat_state_busy();
            break;
        case WIFI_OPENAT_STATE_DONE_OK:
            wifi_openat_state_done_ok();
            break;
        case WIFI_OPENAT_STATE_DONE_ERROR:
            wifi_openat_state_done_error();
            break;
        case WIFI_OPENAT_STATE_WAIT_FOR_DATA:
            wifi_openat_state_wait_for_data();
            break;
        case WIFI_OPENAT_STATE_RECEIVING:
            wifi_openat_receiving();
            break;
        default:
            wifi_openat_state = WIFI_OPENAT_STATE_UNDEFINED;
    }

    /* Check if the state has changed ... */
    if (wifi_openat_state != wifi_openat_state_prev) {
        /* ... Reset Timer */
        wifi_openat_timer = 0;
    } else {
        /* Do nothing */;
    }
}


