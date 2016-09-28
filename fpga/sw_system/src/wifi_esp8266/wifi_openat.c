/* C Standard includes */
#include <string.h>

/* Project includes */
#include "wifi_openat.h"
#include "wifi_uart.h"

/* Definitions */
#define WIFI_OPENAT_UART_BUFFER_SIZE 512

/* Variables */
t_wifi_openat_state wifi_openat_state = WIFI_OPENAT_STATE_UNDEFINED;
u8 wifi_openat_uart_buffer [WIFI_OPENAT_UART_BUFFER_SIZE];
size_t wifi_openat_uart_buffer_size = 0;

/*
 * FSM Interface functions
 */
t_wifi_openat_state wifi_openat_get_state(void) {
    /* Return current state */
    return wifi_openat_state;
}

t_wifi_openat_return wifi_openat_send_cmd (u8 *cmd) {
    t_wifi_openat_return ret;

    /* Check FSM state */
    if (wifi_openat_state == WIFI_OPENAT_STATE_IDLE) {
        /* Send command */
        wifi_uart_write((u8*)cmd, strlen(cmd));

        /* Go to busy state */
        wifi_openat_state = WIFI_OPENAT_STATE_BUSY;

        /* set return */
        ret = WIFI_OPENAT_RETURN_OK;
    } else {
        ret = WIFI_OPENAT_RETURN_NOK;
    }

    return ret;
}

size_t wifi_openat_read (u8 *buf, size_t maxlen) {

    size_t n;
    if (wifi_openat_uart_buffer_size > maxlen) {
        n = maxlen;
    } else {
        n = wifi_openat_uart_buffer_size;
    }

    (void) memcpy(buf, wifi_openat_uart_buffer, wifi_openat_uart_buffer_size);

    wifi_openat_uart_buffer_size = 0;

    wifi_openat_state = WIFI_OPENAT_STATE_IDLE;

    return n;
}

/* FSM state Routines */
void wifi_openat_state_undefined () {
    /* Go to IDLE */
    wifi_openat_state = WIFI_OPENAT_STATE_IDLE;
}

void wifi_openat_state_idle () {
    /* Do nothing, keep being in IDLE */
    wifi_openat_state = WIFI_OPENAT_STATE_IDLE;
}

void wifi_openat_state_busy () {
    u32 n_ok, n_err;

    /* Read response */
    n_ok = wifi_uart_read_OK(wifi_openat_uart_buffer, WIFI_OPENAT_UART_BUFFER_SIZE - 1);
    n_err = wifi_uart_read_ERROR(wifi_openat_uart_buffer, WIFI_OPENAT_UART_BUFFER_SIZE - 1);

    /* Take decision */
    if (n_ok > 0) {
        /* Set last byte to zero */
        wifi_openat_uart_buffer[n_ok] = 0;
        wifi_openat_uart_buffer_size = n_ok;

        /* Change state */
        wifi_openat_state = WIFI_OPENAT_STATE_DONE_OK;
    } else if (n_err > 0) {
        /* Set last byte to zero */
        wifi_openat_uart_buffer[n_err] = 0;
        wifi_openat_uart_buffer_size = n_err;

        /* Change state */
        wifi_openat_state = WIFI_OPENAT_STATE_DONE_ERROR;
    } else {
        /* Do nothing, keep being in BUSY */
        wifi_openat_state = WIFI_OPENAT_STATE_BUSY;
    }


}

void wifi_openat_state_done_ok () {
    /* Do nothing, keep being in DONE_OK */
    wifi_openat_state = WIFI_OPENAT_STATE_DONE_OK;
}

void wifi_openat_state_done_error () {
    /* Do nothing, keep being in DONE_ERROR */
    wifi_openat_state = WIFI_OPENAT_STATE_DONE_ERROR;
}

/*
 * FSM Public routines
 */
void wifi_openat_init (void) {
    /* Init UART Wifi Driver */
    wifi_uart_init();

    /* Initialise FSM state */
    wifi_openat_state = WIFI_OPENAT_STATE_UNDEFINED;

    /* Run FSM task */
    wifi_openat_task();
}

void wifi_openat_task (void) {
    /* Run UART task */
    wifi_uart_task();

    /* Depending on the state run one or other state */
    switch(wifi_openat_state){
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
        default:
            wifi_openat_state = WIFI_OPENAT_STATE_UNDEFINED;
    }
}


