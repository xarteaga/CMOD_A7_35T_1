
#ifndef _WIFI_ESP8266_H
#define _WIFI_ESP8266_H

/* BSP anx Xilinx Includes */
#include "xparameters.h"

#define WIFI_AP_SSID_MAXLEN 16
#define WIFI_AP_MAXNUM      16

typedef enum {
	WIFI_ESP8266_STATE_UNDEFINED,
	WIFI_ESP8266_STATE_CWMODE,
	WIFI_ESP8266_STATE_CWLAP,
	WIFI_ESP8266_STATE_CWJAP,
	WIFI_ESP8266_STATE_GETTING_IP
} t_wifi_esp8266_state;

typedef enum {
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4
} t_wifi_ap_ecn;

typedef struct {
    t_wifi_ap_ecn   ecn;
    uint8_t         ssid[WIFI_AP_SSID_MAXLEN];
    int32_t         rssi;
} t_wifi_ap;

typedef struct {
	size_t count;
	t_wifi_ap entries [WIFI_AP_MAXNUM];
} t_wifi_ap_list;

void wifi_esp8266_init ( void );
void wifi_esp8266_task ( void );

#endif /* _WIFI_ESP8266_H */
