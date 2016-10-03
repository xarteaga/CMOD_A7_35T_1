/* C Standard Includes */
#include <stdio.h>

/* BSP & Xilinx INcludes */
#include "xil_types.h"

/* Project includes */
#include "http_client.h"
#include "scheduler.h"
#include "wifi_esp8266.h"

/* Function prototypes */
scheduler_callback http_client_task_poll(uint32_t elapsed);

/* variables */
static t_http_client_state http_client_state = HTTP_CLIENT_STATE_UNDEFINED;
static scheduler_entry_t http_client_task_poll_entry = {0, 10000, http_client_task_poll};

scheduler_callback http_client_task_poll(u32 elapsed) {
	if (http_client_state == HTTP_CLIENT_STATE_IDLE) {
		/* Send Connect */
		wifi_esp8266_connect("google.com", 80);

		/* Go to connecting */
		http_client_state = HTTP_CLIENT_STATE_CONNECTING;
	}

	/* return nothing */
	return NULL;
}

static void http_client_undefined(void){
	t_wifi_esp8266_state wifi_esp8266_state = wifi_esp8266_get_state();

	if (wifi_esp8266_state == WIFI_ESP8266_STATE_READY) {
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
	uint8_t http_request [] = "GET /gen_204 HTTP/1.1\nConnection: close\n\n";
	t_wifi_esp8266_state wifi_esp8266_state = wifi_esp8266_get_state();

	if (wifi_esp8266_state == WIFI_ESP8266_STATE_CONNECTED) {
		/* Send message */
		wifi_esp8266_send(http_request, sizeof(http_request));

		/* Go to idle */
		http_client_state = HTTP_CLIENT_STATE_SENDING;
	} else {
		/* keep same state */
		http_client_state = HTTP_CLIENT_STATE_CONNECTING;
	}
}

static void http_client_sending (void) {
	t_wifi_esp8266_state wifi_esp8266_state = wifi_esp8266_get_state();

	if (wifi_esp8266_state == WIFI_ESP8266_STATE_READY) {
		/* Go to idle */
		http_client_state = HTTP_CLIENT_STATE_RECEIVING;
	} else {
		/* keep same state */
		http_client_state = HTTP_CLIENT_STATE_SENDING;
	}
}

static void http_client_receiving (void) {
	t_wifi_esp8266_state wifi_esp8266_state = wifi_esp8266_get_state();

	if (wifi_esp8266_state == WIFI_ESP8266_STATE_READY) {
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
		/* Go to idle */
		http_client_state = HTTP_CLIENT_STATE_IDLE;
	} else {
		/* keep same state */
		http_client_state = HTTP_CLIENT_STATE_DISCONNECTING;
	}
}

/* Initiation routine */
void http_client_init ( void ) {
    /* Add polling task to scheduler */
    scheduler_add_entry(&http_client_task_poll_entry);

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
