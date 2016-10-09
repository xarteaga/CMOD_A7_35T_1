/* C Standard Includes */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

/* BSP & Xilinx includes */
#include "xuartlite.h"

/* Project includes */
#include "wifi_esp8266.h"
#include "wifi_openat.h"
#include "scheduler.h"
#include "wifi_utils.h"
#include "wifi_cfg.h"
#include "buffer.h"

/* Defines */
#define WIFI_UART_MAX_BUF   4096
#define WIFI_CMD_MAX_LEN    64

/* Function prototypes */
scheduler_callback wifi_esp8266_task_poll(uint32_t elapsed);

/* Variables */
static u8 wifi_esp8266_buffer[WIFI_UART_MAX_BUF];
static size_t wifi_esp8266_buffer_size;
static scheduler_entry_t wifi_esp8266_task_poll_entry = {0, 5000, wifi_esp8266_task_poll};
static t_wifi_ap_list wifi_ap_list;
static t_wifi_esp8266_state wifi_esp8266_state = WIFI_ESP8266_STATE_UNDEFINED;

static t_buffer *wifi_esp8266_recv_buf = NULL;
static t_buffer *wifi_esp8266_send_buf = NULL;

void wifi_esp8266_connect(uint8_t *addr, uint16_t port, t_buffer *send_buf, t_buffer *recv_buf) {
    uint8_t cmd[WIFI_CMD_MAX_LEN] = {0};

    /* Check state */
    if (wifi_esp8266_state == WIFI_ESP8266_STATE_READY) {
        /* Set buffers */
        wifi_esp8266_send_buf = send_buf;
        wifi_esp8266_recv_buf = recv_buf;
        wifi_openat_set_tcp_recv_buffer(recv_buf);

        /* Build command */
        (void) snprintf((char *) cmd, WIFI_CMD_MAX_LEN, "AT+CIPSTART=4,\"TCP\",\"%s\",%d\r\n", (char *) addr,
                        (int) port);

        /* Send command */
        (void) wifi_openat_send_cmd(cmd);

        /* Print trace */
        xil_printf("[%s] Connecting to %s:%d ...\r\n", __func__, (char *) addr, (int) port);

        /* Go to Linking state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CONNECTING;
    }
}

void wifi_esp8266_disconnect(void) {
    uint8_t cmd[] = "AT+CIPCLOSE=4\r\n";

    if (wifi_esp8266_state == WIFI_ESP8266_STATE_CONNECTED) {
        /* Send command */
        (void) wifi_openat_send_cmd(cmd);

        /* Go to Linking state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_DISCONNECTING;
    }
}

void wifi_esp8266_send_Reset(void) {
    t_wifi_openat_return openat_return = wifi_openat_send_cmd((uint8_t *) "AT+RST\r\n");
    if (openat_return == WIFI_OPENAT_RETURN_NOK) {
        xil_printf("[%s] wifi_openat_send_cmd returned %d\r\n", __FUNCTION__, openat_return);
    }
}

scheduler_callback wifi_esp8266_task_poll(u32 elapsed) {
    if (wifi_esp8266_state == WIFI_ESP8266_STATE_READY) {
        /* Send command */
        t_wifi_openat_return openat_return = wifi_openat_send_cmd((uint8_t *) "AT+CIFSR\r\n");

        /* Check if the command has been put */
        if (openat_return == WIFI_OPENAT_RETURN_NOK) {
            xil_printf("[%s] OpenAT controller is Busy \r\n", __FUNCTION__);
        } else {
            //xil_printf("[%s] wifi_openat_send_cmd OK\r\n", __FUNCTION__);
        }
    }

    /* return nothing */
    return NULL;
}

void wifi_esp8266_undefined(void) {
    /* Set CWMODE
     * mode：
     *  1 means Station mode
     *	2 means AP mode
     *	3 means AP + Station mode
     */
    t_wifi_openat_state wifi_openat_state = wifi_openat_get_state();

    /* Evaluate state */
    if (wifi_openat_state == WIFI_OPENAT_STATE_IDLE) {
        /* Send Command */
        wifi_openat_send_cmd((uint8_t *) "AT+CWMODE=1\r\n");

        xil_printf("[%s] Setting CWMODE to %d\r\n", __FUNCTION__, 1);

        /* Go to next state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CWMODE;
    } else if ((wifi_openat_state == WIFI_OPENAT_STATE_DONE_OK) ||
               (wifi_openat_state == WIFI_OPENAT_STATE_DONE_ERROR)) {
        /* Flush read buffer */
        wifi_esp8266_buffer_size = wifi_openat_read(wifi_esp8266_buffer, WIFI_UART_MAX_BUF - 1);
        wifi_esp8266_buffer[wifi_esp8266_buffer_size] = 0;

        /* Print message */
        xil_printf("[%s] %s\r\n", __FUNCTION__, wifi_esp8266_buffer);

        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_UNDEFINED;
    } else {
        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_UNDEFINED;
    }
}

void wifi_esp8266_cwmode(void) {
    t_wifi_openat_state wifi_openat_state = wifi_openat_get_state();

    /* Evaluate state */
    if (wifi_openat_state == WIFI_OPENAT_STATE_DONE_OK) {
        /* Flush read buffer */
        wifi_esp8266_buffer_size = wifi_openat_read(wifi_esp8266_buffer, WIFI_UART_MAX_BUF - 1);
        wifi_esp8266_buffer[wifi_esp8266_buffer_size] = 0;

        /* Print message */
        xil_printf("[%s] CWMODE returned OK... Scanning APs in range ...\r\n", __FUNCTION__);

        /* Send CWLAP OpenAT command */
        wifi_openat_send_cmd("AT+CWLAP\r\n");

        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CWLAP;
    } else if ((wifi_openat_state == WIFI_OPENAT_STATE_DONE_ERROR) ||
               (wifi_openat_state == WIFI_OPENAT_STATE_IDLE)) {
        uint8_t cmd[] = "AT+CWMODE=1\r\n";

        /* Re-send OpenAT command */
        wifi_openat_send_cmd(cmd);

        /* Print message */
        xil_printf("[%s] CWMODE returned FAIL... Resending CWMODE ...\r\n", __FUNCTION__);

        /* Keep being on the same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CWMODE;
    } else {
        /* Keep being on the same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CWMODE;
    }
}

static void wifi_esp8266_cwlap(void) {
    uint32_t k;
    uint8_t ap_ssid_detected = FALSE;
    t_wifi_openat_state wifi_openat_state = wifi_openat_get_state();
    uint8_t cmd[WIFI_CMD_MAX_LEN] = {0};
    size_t size;

    /* Check if it is performed OK */
    if (wifi_openat_state == WIFI_OPENAT_STATE_DONE_OK) {
        size = wifi_openat_read(wifi_esp8266_buffer, WIFI_UART_MAX_BUF - 1);

        /* Parse received buffer */
        (void) wifi_utils_parse_cwlap(wifi_esp8266_buffer, size, &wifi_ap_list);

        //xil_printf("[%s] %s", __FUNCTION__, wifi_esp8266_buffer);

        /* Look for the desired AP */
        for (k = 0; (k < wifi_ap_list.count) && (ap_ssid_detected == FALSE); k++) {
            xil_printf("[%s] WiFi AP '%s' \r\n", __FUNCTION__, (char *) wifi_ap_list.entries[k].ssid);

            /* Compare */
            if (strncmp((char *) wifi_ap_list.entries[k].ssid,
                        WIFI_CFG_SSID, WIFI_AP_SSID_MAXLEN) == 0) {
                ap_ssid_detected = TRUE;

                /* Send Debug trace */
                xil_printf("[%s] WiFi AP '%s' has been detected. Joining...\r\n", __FUNCTION__, WIFI_CFG_SSID);
            }
        }
    }

    /* Evaluate state */
    if (ap_ssid_detected == TRUE) {
        (void) snprintf((char *) cmd, WIFI_CMD_MAX_LEN, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_CFG_SSID, WIFI_CFG_PWD);

        (void) wifi_openat_send_cmd(cmd);

        /* Join Access Point */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CWJAP;
    } else if ((ap_ssid_detected != TRUE) &&
               (wifi_openat_state == WIFI_OPENAT_STATE_IDLE)) {
        /* Re-send OpenAT command */
        (void) wifi_openat_send_cmd("AT+CWLAP\r\n");

        /* Print message */
        xil_printf("[%s] The desired AP was not found... Resending CWLAP ...\r\n", __FUNCTION__);

        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CWLAP;
    } else if ((ap_ssid_detected != TRUE) &&
               (wifi_openat_state == WIFI_OPENAT_STATE_DONE_ERROR)) {

        /* Re-send OpenAT command */
        (void) wifi_openat_send_cmd((uint8_t) "AT+CWLAP\r\n");

        /* Print message */
        xil_printf("[%s] CWLAP returned FAIL... Resending CWLAP ...\r\n", __FUNCTION__);

        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CWLAP;
    } else {
        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CWLAP;
    }
}

void wifi_esp8266_cwjap(void) {
    t_wifi_openat_state wifi_openat_state = wifi_openat_get_state();
    uint8_t cmd[WIFI_CMD_MAX_LEN] = {0};

    /* Evaluate state */
    if (wifi_openat_state == WIFI_OPENAT_STATE_DONE_OK) {
        /* Flush read buffer */
        wifi_esp8266_buffer_size = wifi_openat_read(wifi_esp8266_buffer, WIFI_UART_MAX_BUF - 1);
        wifi_esp8266_buffer[wifi_esp8266_buffer_size] = 0;

        /* Print message */
        xil_printf("[%s] Connected to AP with SSID '%s'...\r\n", __FUNCTION__, WIFI_CFG_SSID);

        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_GETTING_IP;
    } else if ((wifi_openat_state == WIFI_OPENAT_STATE_DONE_ERROR) ||
               (wifi_openat_state == WIFI_OPENAT_STATE_IDLE)) {
        /* Re-send OpenAT command */
        (void) snprintf((char *) cmd, WIFI_CMD_MAX_LEN, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_CFG_SSID, WIFI_CFG_PWD);
        (void) wifi_openat_send_cmd(cmd);

        /* Send debug trace */
        xil_printf("[%s] Re-sending command for joining AP...\r\n", __FUNCTION__);

        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CWJAP;
    } else {
        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CWJAP;
    }
}

void wifi_esp8266_getting_ip(void) {
    t_wifi_openat_state wifi_openat_state = wifi_openat_get_state();

    /* Evaluate state */
    if (wifi_openat_state == WIFI_OPENAT_STATE_DONE_OK) {
        /* Flush read buffer */
        wifi_esp8266_buffer_size = wifi_openat_read(wifi_esp8266_buffer, WIFI_UART_MAX_BUF - 1);
        wifi_esp8266_buffer[wifi_esp8266_buffer_size] = 0;

        /* TODO: parse IP address */

        /* Print message */
        xil_printf("[%s] %s\r\n", __FUNCTION__, wifi_esp8266_buffer);

        /* Configure for multiple connections */
        (void) wifi_openat_send_cmd((uint8_t *) "AT+CIPMUX=1\r\n");

        /* Go to the next state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CIPMUX;
    } else if ((wifi_openat_state == WIFI_OPENAT_STATE_DONE_ERROR) ||
               (wifi_openat_state == WIFI_OPENAT_STATE_IDLE)) {
        /* Re-send command */
        (void) wifi_openat_send_cmd((uint8_t *) "AT+CIFSR\r\n");

        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_GETTING_IP;
    } else {
        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_GETTING_IP;
    }
}

void wifi_esp8266_cipmux(void) {
    t_wifi_openat_state wifi_openat_state = wifi_openat_get_state();

    /* Evaluate state */
    if (wifi_openat_state == WIFI_OPENAT_STATE_DONE_OK) {
        /* Flush read buffer */
        wifi_esp8266_buffer_size = wifi_openat_read(wifi_esp8266_buffer, WIFI_UART_MAX_BUF - 1);
        wifi_esp8266_buffer[wifi_esp8266_buffer_size] = 0;

        /* TODO: parse IP address */

        /* Print message */
        xil_printf("[%s] %s\r\n", __FUNCTION__, wifi_esp8266_buffer);

        /* Go to the next state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_READY;
    } else if ((wifi_openat_state == WIFI_OPENAT_STATE_DONE_ERROR) ||
               (wifi_openat_state == WIFI_OPENAT_STATE_IDLE)) {
        /* Re-send command */
        (void) wifi_openat_send_cmd((uint8_t *) "AT+CIPMUX=1\r\n");

        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CIPMUX;
    } else {
        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CIPMUX;
    }
}

void wifi_esp8266_connecting(void) {
    char pattern[] = "ALREADY CONNECTED";
    uint32_t i, n;
    t_wifi_openat_state wifi_openat_state = wifi_openat_get_state();
    uint8_t already_connected = FALSE;

    if (wifi_openat_state == WIFI_OPENAT_STATE_DONE_OK) {
        /* Flush read buffer */
        wifi_esp8266_buffer_size = wifi_openat_read(wifi_esp8266_buffer, WIFI_UART_MAX_BUF - 1);
        wifi_esp8266_buffer[wifi_esp8266_buffer_size] = 0;

        already_connected = TRUE;
    } else if ((wifi_openat_state == WIFI_OPENAT_STATE_DONE_ERROR) ||
        (wifi_openat_state == WIFI_OPENAT_STATE_IDLE)) {
        /* Flush read buffer */
        wifi_esp8266_buffer_size = wifi_openat_read(wifi_esp8266_buffer, WIFI_UART_MAX_BUF - 1);
        wifi_esp8266_buffer[wifi_esp8266_buffer_size] = 0;

        if (wifi_esp8266_buffer_size > sizeof(pattern)) {
            n = wifi_esp8266_buffer_size - sizeof(pattern) - 1;
            for (i = 0; (i < n) && (already_connected == FALSE); i++) {
                if (strncmp(pattern, wifi_esp8266_buffer + i, sizeof(pattern) - 1) == 0) {
                    already_connected = TRUE;
                }
            }
        }

    }

    /* Evaluate state */
    if ((wifi_openat_state == WIFI_OPENAT_STATE_DONE_OK) || (already_connected == TRUE)) {
        /* Print message */
        xil_printf("[%s] %s\r\n", __FUNCTION__, wifi_esp8266_buffer);

        /* Success, go to connected */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CONNECTED;
    } else if ((wifi_openat_state == WIFI_OPENAT_STATE_DONE_ERROR) ||
               (wifi_openat_state == WIFI_OPENAT_STATE_IDLE)) {
        /* Print message */
        xil_printf("[%s] Error connecting, going to IDLE...\r\n%s\r\n", __FUNCTION__, wifi_esp8266_buffer);

        /* Failed, go back to ready */
        wifi_esp8266_state = WIFI_ESP8266_STATE_READY;
    } else {
        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CONNECTING;
    }
}

void wifi_esp8266_connected(void) {
    uint8_t cmd[WIFI_CMD_MAX_LEN] = {0};
    uint8_t data[WIFI_ESP8266_MAX_MSG_LEN];
    size_t n_send = buffer_available(wifi_esp8266_send_buf);

    /* Evaluate state */
    if (n_send > 0) {
        n_send = buffer_read(wifi_esp8266_send_buf, data, n_send);

        /* Build command */
        (void) snprintf((char *) cmd, WIFI_CMD_MAX_LEN, "AT+CIPSEND=4,%d\r\n", (int) n_send);

        /* Send command */
        (void) wifi_openat_send_data(cmd, data, n_send);

        /* Print trace */
        xil_printf("[%s] Sending to %d bytes ...\r\n", __FUNCTION__, (int) n_send);

        /* Go to Linking state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_SENDING;
    } else {
        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CONNECTED;
    }
}

void wifi_esp8266_sending(void) {
    t_wifi_openat_state wifi_openat_state = wifi_openat_get_state();

    /* Evaluate state */
    if (wifi_openat_state == WIFI_OPENAT_STATE_DONE_OK) {
        /* Flush read buffer */
        wifi_esp8266_buffer_size = wifi_openat_read(wifi_esp8266_buffer, WIFI_UART_MAX_BUF - 1);
        wifi_esp8266_buffer[wifi_esp8266_buffer_size] = 0;

        /* Print message */
        xil_printf("[%s] Send data OK, going to CONNECTED...\r\n%s\r\n", __FUNCTION__, wifi_esp8266_buffer);

        /* Success, go to connected */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CONNECTED;
    } else if ((wifi_openat_state == WIFI_OPENAT_STATE_DONE_ERROR) ||
               (wifi_openat_state == WIFI_OPENAT_STATE_IDLE)) {

        /* Flush read buffer */
        wifi_esp8266_buffer_size = wifi_openat_read(wifi_esp8266_buffer, WIFI_UART_MAX_BUF - 1);
        wifi_esp8266_buffer[wifi_esp8266_buffer_size] = 0;

        /* Print message */
        xil_printf("[%s] Send data failed, going to CONNECTED...\r\n%s\r\n", __FUNCTION__, wifi_esp8266_buffer);

        /* Failed, go back to ready */
        wifi_esp8266_state = WIFI_ESP8266_STATE_CONNECTED;
    } else {
        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_SENDING;
    }
}

void wifi_esp8266_ready(void) {
    t_wifi_openat_state wifi_openat_state = wifi_openat_get_state();

    /* Evaluate state */
    if (wifi_openat_state == WIFI_OPENAT_STATE_DONE_OK) {
        /* Flush read buffer */
        wifi_esp8266_buffer_size = wifi_openat_read(wifi_esp8266_buffer, WIFI_UART_MAX_BUF - 1);
        wifi_esp8266_buffer[wifi_esp8266_buffer_size] = 0;

        /* Print message */
        xil_printf("[%s] ---\r\n %s\r\n---\r\n", __FUNCTION__, wifi_esp8266_buffer);

        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_READY;
    }
}

static void wifi_esp8266_disconnecting(void) {
    uint8_t cmd[] = "AT+CIPCLOSE=4\r\n";
    t_wifi_openat_state wifi_openat_state = wifi_openat_get_state();

    /* Evaluate state */
    if (wifi_openat_state == WIFI_OPENAT_STATE_DONE_OK) {
        /* Flush read buffer */
        wifi_esp8266_buffer_size = wifi_openat_read(wifi_esp8266_buffer, WIFI_UART_MAX_BUF - 1);
        wifi_esp8266_buffer[wifi_esp8266_buffer_size] = 0;

        /* Print message */
        xil_printf("[%s] TCP Connection closed... \r\n%s\r\n", __FUNCTION__, wifi_esp8266_buffer);

        /* Success, go to connected */
        wifi_esp8266_state = WIFI_ESP8266_STATE_READY;
    } else if ((wifi_openat_state == WIFI_OPENAT_STATE_DONE_ERROR) ||
               (wifi_openat_state == WIFI_OPENAT_STATE_IDLE)) {
        /* Flush read buffer */
        wifi_esp8266_buffer_size = wifi_openat_read(wifi_esp8266_buffer, WIFI_UART_MAX_BUF - 1);
        wifi_esp8266_buffer[wifi_esp8266_buffer_size] = 0;
        xil_printf("[%s] TCP Connection close failed... Trying\r\n%s\r\n", __FUNCTION__, wifi_esp8266_buffer);

        /* Resend TCP close */
        wifi_openat_send_cmd(cmd);

        /* Failed, go back to disconnecting */
        wifi_esp8266_state = WIFI_ESP8266_STATE_DISCONNECTING;
    } else {
        /* Keep same state */
        wifi_esp8266_state = WIFI_ESP8266_STATE_DISCONNECTING;
    }
}

t_wifi_esp8266_state wifi_esp8266_get_state(void) {
    return wifi_esp8266_state;
}

/* Initiation routine */
void wifi_esp8266_init(void) {
    /* Init slave modules */
    wifi_openat_init();

    /* Init AP list */
    wifi_ap_list.count = 0;

    /* Add polling task to scheduler */
    //scheduler_add_entry(&wifi_esp8266_task_poll_entry);

    xil_printf("%32s ... OK\r\n", __func__);
}

void wifi_esp8266_task(void) {
    /* Call Slave tasks */
    wifi_openat_task();

    /* Execute this task FSM */
    switch (wifi_esp8266_state) {
        case WIFI_ESP8266_STATE_UNDEFINED:
            wifi_esp8266_undefined();
            break;
        case WIFI_ESP8266_STATE_CWMODE:
            wifi_esp8266_cwmode();
            break;
        case WIFI_ESP8266_STATE_CWLAP:
            wifi_esp8266_cwlap();
            break;
        case WIFI_ESP8266_STATE_CWJAP:
            wifi_esp8266_cwjap();
            break;
        case WIFI_ESP8266_STATE_GETTING_IP:
            wifi_esp8266_getting_ip();
            break;
        case WIFI_ESP8266_STATE_CIPMUX:
            wifi_esp8266_cipmux();
            break;
        case WIFI_ESP8266_STATE_READY:
            wifi_esp8266_ready();
            break;
        case WIFI_ESP8266_STATE_CONNECTING:
            wifi_esp8266_connecting();
            break;
        case WIFI_ESP8266_STATE_CONNECTED:
            wifi_esp8266_connected();
            break;
        case WIFI_ESP8266_STATE_SENDING:
            wifi_esp8266_sending();
            break;
        case WIFI_ESP8266_STATE_DISCONNECTING:
            wifi_esp8266_disconnecting();
            break;
        default:
            wifi_esp8266_state = WIFI_ESP8266_STATE_UNDEFINED;
    }
}
