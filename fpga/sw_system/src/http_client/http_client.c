/* C Standard Includes */
#include <stdio.h>
#include <wifi_openat.h>

/* BSP & Xilinx INcludes */
#include "xil_types.h"

/* Project includes */
#include "wifi_cfg.h"
#include "buffer.h"
#include "http_client.h"
#include "scheduler.h"
#include "wifi_esp8266.h"

/* Function prototypes */
scheduler_callback http_client_task_poll(uint32_t elapsed);

/* variables */
static t_buffer http_client_buffer_send;
static t_buffer http_client_buffer_recv;
static t_http_client_state http_client_state = HTTP_CLIENT_STATE_UNDEFINED;
static scheduler_entry_t http_client_task_poll_entry = {0, 10000, http_client_task_poll};

scheduler_callback http_client_task_poll(u32 elapsed) {
	if (http_client_state == HTTP_CLIENT_STATE_IDLE) {
		xil_printf("[%s] HTTP Client connecting...\r\n", __FUNCTION__);

		/* Send Connect */
		wifi_esp8266_connect((uint8_t*)HTTP_CLIENT_SERVER_ADDR, HTTP_CLIENT_SERVER_PORT, &http_client_buffer_send,
                             &http_client_buffer_recv);

		/* Go to connecting */
		http_client_state = HTTP_CLIENT_STATE_CONNECTING;
	}

	/* return nothing */
	return NULL;
}

static void http_client_undefined(void){
	t_wifi_esp8266_state wifi_esp8266_state = wifi_esp8266_get_state();

	if (wifi_esp8266_state == WIFI_ESP8266_STATE_READY) {
		xil_printf("[%s] HTTP Client detected WiFi module ready.\r\n", __FUNCTION__);

		/* Go to idle */
		http_client_state = HTTP_CLIENT_STATE_IDLE;
	} else {
		/* keep same state */
		http_client_state = HTTP_CLIENT_STATE_UNDEFINED;
	}
}

static void http_client_idle (void) {
	t_wifi_esp8266_state wifi_esp8266_state = wifi_esp8266_get_state();

	if (wifi_esp8266_state < WIFI_ESP8266_STATE_READY) {
		/*  */
		xil_printf("[%s] WiFi ESP8266 module is not ready anymore. HTTP client is NOT available anymore.\r\n", __FUNCTION__);

		/* Go to idle */
		http_client_state = HTTP_CLIENT_STATE_UNDEFINED;
	} else {
		/* keep same state */
		http_client_state = HTTP_CLIENT_STATE_IDLE;
	}
}

static void http_client_connecting (void) {
	uint8_t http_request [] = "GET /gen_204 HTTP/1.1\r\nHost: www.google.com\r\nUser-Agent: VaXiOS\r\nAccept: */*\r\n\n";
	t_wifi_esp8266_state wifi_esp8266_state = wifi_esp8266_get_state();

	if (wifi_esp8266_state == WIFI_ESP8266_STATE_CONNECTED) {
        xil_printf("[%s] HTTP Client connected... Sending request... \r\n", __FUNCTION__);

        /* Send message */
        buffer_write_string(&http_client_buffer_send, http_request);

        /* Go to sending */
        http_client_state = HTTP_CLIENT_STATE_SENDING;
    }else if (wifi_esp8266_state <= WIFI_ESP8266_STATE_READY) {
        /* Print trace */
        xil_printf("[%s] HTTP Client connection failed... Retrying...\r\n", __FUNCTION__);

        /* Send Connect */
        wifi_esp8266_connect((uint8_t*)HTTP_CLIENT_SERVER_ADDR, HTTP_CLIENT_SERVER_PORT, &http_client_buffer_send,
                             &http_client_buffer_recv);
        http_client_state = HTTP_CLIENT_STATE_CONNECTING;
	} else {
		/* keep same state */
		http_client_state = HTTP_CLIENT_STATE_CONNECTING;
	}
}

static void http_client_sending (void) {
	t_wifi_esp8266_state wifi_esp8266_state = wifi_esp8266_get_state();

    if (wifi_esp8266_state == WIFI_ESP8266_STATE_CONNECTED) {
        xil_printf("[%s] HTTP Request sent... Waiting for reply... \r\n", __FUNCTION__);

        /* Go to idle */
        http_client_state = HTTP_CLIENT_STATE_RECEIVING;
    } else if ((wifi_esp8266_state != WIFI_ESP8266_STATE_SENDING) &&
            (wifi_esp8266_state != WIFI_ESP8266_STATE_CONNECTED)) {
        xil_printf("[%s] WiFi Module is not Conencting anymore, going to IDLE...\r\n", __FUNCTION__);

        /* Go to idle */
        http_client_state = HTTP_CLIENT_STATE_IDLE;
	} else {
		/* keep same state */
		http_client_state = HTTP_CLIENT_STATE_SENDING;
	}
}

static void http_client_receiving (void) {
    uint8_t temp[2048];
    size_t n = 0;

    if (buffer_available(&http_client_buffer_recv) > 0) {
        n = buffer_read(&http_client_buffer_recv, temp, 2048);
        temp[n] = 0;

        if (n > 0){
            xil_printf("[%s] HTTP Reply received: \r\n%s\r\n", __FUNCTION__, temp);
        }

    }

	if (n > 0) {
        xil_printf("[%s] HTTP Reply received... Disconnecting... \r\n", __FUNCTION__);

        wifi_esp8266_disconnect();

		/* Go to idle */
		http_client_state = HTTP_CLIENT_STATE_DISCONNECTING;
	} else {
		/* keep same state */
		http_client_state = HTTP_CLIENT_STATE_RECEIVING;
	}
}

static void http_client_disconnecting (void) {
	t_wifi_esp8266_state wifi_esp8266_state = wifi_esp8266_get_state();

	if (wifi_esp8266_state == WIFI_ESP8266_STATE_READY) {
        xil_printf("[%s] HTTP Client Disconnected... \r\n", __FUNCTION__);

		/* Go to idle */
		http_client_state = HTTP_CLIENT_STATE_IDLE;
	} else {
		/* keep same state */
		http_client_state = HTTP_CLIENT_STATE_DISCONNECTING;
	}
}

/* Initiation routine */
void http_client_init ( void ) {
    t_buffer_return err;

    /* Add polling task to scheduler */
    scheduler_add_entry(&http_client_task_poll_entry);

    /* Initialise buffers */
    err = buffer_create(&http_client_buffer_send, HTTP_CLIENT_SEND_BUFFER_SIZE);
    if (err < BUFFER_ERROR_OK) {
        buffer_print_error(__FUNCTION__, &http_client_buffer_send);
    }
    err = buffer_create(&http_client_buffer_recv, HTTP_CLIENT_SEND_BUFFER_SIZE);
    if (err < BUFFER_ERROR_OK) {
        buffer_print_error(__FUNCTION__, &http_client_buffer_recv);
    }

    /* Set undefined state by default */
    http_client_state = HTTP_CLIENT_STATE_UNDEFINED;

    xil_printf("%32s ... OK\r\n", __func__);
}

void http_client_task ( void ) {
    /* Execute this task FSM */
    switch(http_client_state){
    case HTTP_CLIENT_STATE_UNDEFINED:
    	http_client_undefined();
    	break;
    case HTTP_CLIENT_STATE_CONNECTING:
    	http_client_connecting();
    	break;
    case HTTP_CLIENT_STATE_IDLE:
    	http_client_idle();
    	break;
    case HTTP_CLIENT_STATE_RECEIVING:
    	http_client_receiving();
    	break;
    case HTTP_CLIENT_STATE_SENDING:
    	http_client_sending();
    	break;
    case HTTP_CLIENT_STATE_DISCONNECTING:
    	http_client_disconnecting();
    	break;
    default:
    	http_client_state = HTTP_CLIENT_STATE_UNDEFINED;
    }
}
