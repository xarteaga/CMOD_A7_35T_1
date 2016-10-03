/* C Standard includes */
#include <string.h>
#include <stdio.h>

/* Project includes */
#include "wifi_openat.h"
#include "wifi_uart.h"
#include "scheduler.h"

/* Definitions */
#define WIFI_OPENAT_UART_BUFFER_SIZE 512
#define WIFI_OPENAT_CMD_TIMEOUT      20000

/* Function prototypes */
scheduler_callback wifi_openat_task_timer(u32 elapsed);

/* Variables */
t_wifi_openat_state wifi_openat_state = WIFI_OPENAT_STATE_UNDEFINED;
u8 wifi_openat_uart_buffer[WIFI_OPENAT_UART_BUFFER_SIZE];
size_t wifi_openat_uart_buffer_size = 0;
static scheduler_entry_t wifi_openat_task_timer_entry = {0, 10, wifi_openat_task_timer};
static u32 wifi_openat_timer = 0;

/*
 * FSM Interface functions
 */
t_wifi_openat_state wifi_openat_get_state(void) {

    /*if (wifi_openat_state == WIFI_OPENAT_STATE_DONE_ERROR) {
        wifi_openat_state = WIFI_OPENAT_STATE_IDLE;
    } else if (wifi_openat_state == WIFI_OPENAT_STATE_DONE_OK) {
        wifi_openat_state = WIFI_OPENAT_STATE_IDLE;
    }*/

    /* Return current state */
    return wifi_openat_state;
}

void wifi_openat_write(uint8_t *buf, size_t nbytes) {
	/* Send command */
	wifi_uart_write(buf, nbytes);
}

t_wifi_openat_return wifi_openat_send_cmd(uint8_t *cmd) {
    t_wifi_openat_return ret;

    /* Check FSM state */
    if (wifi_openat_state == WIFI_OPENAT_STATE_IDLE) {
        /* Send command */
        wifi_uart_write(cmd, strlen(cmd));

        /* Go to busy state */
        wifi_openat_state = WIFI_OPENAT_STATE_BUSY;

        /* set return */
        ret = WIFI_OPENAT_RETURN_OK;
    } else {
        ret = WIFI_OPENAT_RETURN_NOK;
    }

    return ret;
}

size_t wifi_openat_read(u8 *buf, size_t maxlen) {

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
scheduler_callback wifi_openat_task_timer(u32 elapsed) {
    wifi_openat_timer += elapsed;

    return NULL;
}

void wifi_openat_state_undefined() {
    /* Go to IDLE */
    wifi_openat_state = WIFI_OPENAT_STATE_IDLE;
}

void wifi_openat_state_idle() {
    /* Do nothing, keep being in IDLE */
    wifi_openat_state = WIFI_OPENAT_STATE_IDLE;
}

void wifi_openat_state_busy() {
    u32 n_ok, n_err = 0;

    /* Read response */
    n_ok = wifi_uart_read_key(wifi_openat_uart_buffer, WIFI_OPENAT_UART_BUFFER_SIZE - 1, (uint8_t*)"\r\nOK\r\n");
    if (n_ok == 0) {
    	n_err = wifi_uart_read_ERROR(wifi_openat_uart_buffer, WIFI_OPENAT_UART_BUFFER_SIZE - 1);
    }

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
    } else if (wifi_openat_timer > WIFI_OPENAT_CMD_TIMEOUT) {
        xil_printf("[%s] Timeout \r\n", __FUNCTION__);

        /* Change state */
        wifi_openat_state = WIFI_OPENAT_STATE_DONE_ERROR;
    } else {
        /* Do nothing, keep being in BUSY */
        wifi_openat_state = WIFI_OPENAT_STATE_BUSY;
    }
}

void wifi_openat_state_done_ok() {
    /* Do nothing, keep being in DONE_OK */
    wifi_openat_state = WIFI_OPENAT_STATE_DONE_OK;
}

void wifi_openat_state_done_error() {
    /* Do nothing, keep being in DONE_ERROR */
    wifi_openat_state = WIFI_OPENAT_STATE_IDLE;
}

/*
 * FSM Public routines
 */
void wifi_openat_init(void) {
    /* Init UART Wifi Driver */
    wifi_uart_init();

    /* Add task entry to the scheduler */
    scheduler_add_entry(&wifi_openat_task_timer_entry);

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


