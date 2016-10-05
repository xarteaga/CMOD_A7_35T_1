/* C Standard includes */
#include <string.h>
#include <stdio.h>

/* Project includes */
#include "wifi_openat.h"
#include "wifi_uart.h"
#include "scheduler.h"

/* Definitions */
#define WIFI_OPENAT_UART_BUFFER_SIZE 512
#define WIFI_OPENAT_RECV_BUFFER_SIZE 4096
#define WIFI_OPENAT_CMD_TIMEOUT      30000

/* Function prototypes */
scheduler_callback wifi_openat_task_timer(u32 elapsed);

/* Variables */
static t_wifi_openat_state wifi_openat_state = WIFI_OPENAT_STATE_UNDEFINED;
static u8 wifi_openat_uart_buffer[WIFI_OPENAT_UART_BUFFER_SIZE];
static size_t wifi_openat_uart_buffer_size = 0;
static scheduler_entry_t wifi_openat_task_timer_entry = {0, 10, wifi_openat_task_timer};
static u32 wifi_openat_timer = 0;

static u8 wifi_openat_recv_buffer[WIFI_OPENAT_RECV_BUFFER_SIZE];
static size_t wifi_openat_recv_buffer_size;

static u8 wifi_openat_send_buffer[WIFI_OPENAT_RECV_BUFFER_SIZE];
static size_t wifi_openat_send_buffer_size;


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

t_wifi_openat_return wifi_openat_send_cmd(uint8_t *cmd) {
    t_wifi_openat_return ret;

    /* Check FSM state */
    if (wifi_openat_state == WIFI_OPENAT_STATE_IDLE) {

        /* Send command */
        wifi_uart_write(cmd, strlen((char*)cmd));

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
        wifi_uart_write(cmd, strlen((char*)cmd));

        /* Go to busy state */
        wifi_openat_state = WIFI_OPENAT_STATE_WAIT_FOR_DATA;

        /* set return */
        ret = WIFI_OPENAT_RETURN_OK;
    } else {
        ret = WIFI_OPENAT_RETURN_NOK;
    }

    return ret;
}

size_t wifi_openat_recv(uint8_t* buf) {
    return 0;
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
    uint8_t cmd[] = "AT+RST\r\n";
    wifi_uart_write(cmd, sizeof(cmd));

    /* Go to IDLE */
    wifi_openat_state = WIFI_OPENAT_STATE_IDLE;
}

void wifi_openat_state_idle() {
    size_t n_recv = wifi_uart_read(wifi_openat_recv_buffer, WIFI_OPENAT_RECV_BUFFER_SIZE - 1);
    if (n_recv > 0) {
        wifi_openat_recv_buffer[n_recv] = 0;
        wifi_openat_recv_buffer_size = n_recv;

        xil_printf("[%s] Data received\r\n%s\r\n", __FUNCTION__, wifi_openat_recv_buffer);

        wifi_openat_state = WIFI_OPENAT_STATE_IDLE;
    }else {
        /* Do nothing, keep being in IDLE */
        wifi_openat_state = WIFI_OPENAT_STATE_IDLE;
    }
}

void wifi_openat_state_busy() {
    u32 n_ok, n_err = 0;

    /* Read response */
    n_ok = wifi_uart_read_key(wifi_openat_uart_buffer, WIFI_OPENAT_UART_BUFFER_SIZE - 1, (uint8_t*)"OK\r\n");
    if (n_ok == 0) {
    	n_err = wifi_uart_read_key(wifi_openat_uart_buffer, WIFI_OPENAT_UART_BUFFER_SIZE - 1, (uint8_t*)"ERROR\r\n");
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

static void wifi_openat_state_done_ok() {
    /* Do nothing, keep being in DONE_OK */
    wifi_openat_state = WIFI_OPENAT_STATE_DONE_OK;
}

static void wifi_openat_state_done_error() {
    /* Do nothing, keep being in DONE_ERROR */
    wifi_openat_state = WIFI_OPENAT_STATE_IDLE;
}

static void wifi_openat_state_wait_for_data() {
    size_t n_ok = wifi_uart_read_key(wifi_openat_uart_buffer, WIFI_OPENAT_UART_BUFFER_SIZE, ">");

    if (n_ok > 0) {
        /* Print trace */
        xil_printf("[%s] Ready for sending data\r\n%s\r\n", __FUNCTION__, wifi_openat_send_buffer);

        /* Send stored data */
        wifi_uart_write(wifi_openat_send_buffer, wifi_openat_send_buffer_size);

        /* Do nothing, keep being in DONE_ERROR */
        wifi_openat_state = WIFI_OPENAT_STATE_BUSY;

    }else {
        /* Do nothing, keep being in DONE_ERROR */
        wifi_openat_state = WIFI_OPENAT_STATE_WAIT_FOR_DATA;
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


