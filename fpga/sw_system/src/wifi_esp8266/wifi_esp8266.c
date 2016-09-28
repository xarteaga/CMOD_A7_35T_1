/* C Standard Includes */
#include <stdio.h>

/* BSP & Xilinx includes */
#include "xuartlite.h"

/* Project includes */
#include "wifi_esp8266.h"
#include "wifi_openat.h"
#include "scheduler.h"

#define WIFI_UART_MAX_BUF 1024

/* Function prototypes */
scheduler_callback wifi_esp8266_task_poll(u32 elapsed);

/* Variables */
static u8 wifi_esp8266_buffer [WIFI_UART_MAX_BUF];
static scheduler_entry_t wifi_esp8266_task_poll_entry = {0, 10000, wifi_esp8266_task_poll};


void wifi_esp8266_send_CWLAP(void) {
    u8 cmd [] = "AT+CWLAP\r\n";
    t_wifi_openat_return openat_return = wifi_openat_send_cmd(cmd);
    if (openat_return == WIFI_OPENAT_RETURN_NOK) {
        xil_printf("[%s] wifi_openat_send_cmd returned %d\r\n", __FUNCTION__, openat_return);
    }
}

void wifi_esp8266_send_Reset(void) {
    u8 cmd [] = "AT+RST\r\n";

    t_wifi_openat_return openat_return = wifi_openat_send_cmd(cmd);
    if (openat_return == WIFI_OPENAT_RETURN_NOK) {
        xil_printf("[%s] wifi_openat_send_cmd returned %d\r\n", __FUNCTION__, openat_return);
    }
}

scheduler_callback wifi_esp8266_task_poll(u32 elapsed) {
    wifi_esp8266_send_CWLAP();
}

/* Initiation routine */
void wifi_esp8266_init ( void ) {
    wifi_openat_init();

    /* Add polling task to scheduler */
    scheduler_add_entry(&wifi_esp8266_task_poll_entry);

    /* Reset WiFi Module */
    wifi_esp8266_send_Reset();

    xil_printf("%s ... OK\r\n", __func__);
}

void wifi_esp8266_task ( void ) {
    u32 size = 0;

    /* Wifi Tasks */
    wifi_openat_task();


    if (wifi_openat_get_state() == WIFI_OPENAT_STATE_DONE_OK){
        size = wifi_openat_read(wifi_esp8266_buffer, WIFI_UART_MAX_BUF - 1);
        wifi_esp8266_buffer[size] = 0;

        xil_printf("%s", wifi_esp8266_buffer);
    } else if (wifi_openat_get_state() == WIFI_OPENAT_STATE_DONE_ERROR) {
        xil_printf("[%s] WIFI_OPENAT_STATE_DONE_ERROR\r\n", __FUNCTION__);
    }
}
