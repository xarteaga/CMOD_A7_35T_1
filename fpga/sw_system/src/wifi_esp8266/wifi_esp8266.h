
#ifndef _WIFI_ESP8266_H
#define _WIFI_ESP8266_H

/* BSP anx Xilinx Includes */
#include "xparameters.h"

/* Project includes */
#include "wifi_cfg.h"
#include "buffer.h"

typedef enum {
	WIFI_ESP8266_STATE_UNDEFINED = 0,
	WIFI_ESP8266_STATE_CWMODE,
	WIFI_ESP8266_STATE_CWLAP,
	WIFI_ESP8266_STATE_CWJAP,
	WIFI_ESP8266_STATE_GETTING_IP,
	WIFI_ESP8266_STATE_CIPMUX,
	WIFI_ESP8266_STATE_READY,
	WIFI_ESP8266_STATE_CONNECTING,
	WIFI_ESP8266_STATE_CONNECTED,
	WIFI_ESP8266_STATE_SENDING,
	WIFI_ESP8266_STATE_DISCONNECTING
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

t_wifi_esp8266_state wifi_esp8266_get_state(void);
void wifi_esp8266_init ( void );
void wifi_esp8266_task ( void );

void wifi_esp8266_connect(uint8_t *addr, uint16_t port, t_buffer *send_buf, t_buffer *recv_buf);
void wifi_esp8266_disconnect(void);

#endif /* _WIFI_ESP8266_H */
