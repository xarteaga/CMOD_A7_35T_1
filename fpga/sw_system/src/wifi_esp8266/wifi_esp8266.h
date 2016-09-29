
#ifndef _WIFI_ESP8266_H
#define _WIFI_ESP8266_H

/* BSP anx Xilinx Includes */
#include "xparameters.h"

#define WIFI_AP_SSID_MAXLEN 16
#define WIFI_AP_MAXNUM      16

typedef struct {
    uint8_t     ssid[WIFI_AP_SSID_MAXLEN];
    int16_t     rssi;
} t_wifi_ap;

typedef t_wifi_ap t_wifi_ap_list [WIFI_AP_MAXNUM];

void wifi_esp8266_send_CWLAP(void);

void wifi_esp8266_init ( void );
void wifi_esp8266_task ( void );

#endif /* _WIFI_ESP8266_H */
