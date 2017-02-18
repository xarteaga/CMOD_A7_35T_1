/*
 * wifi.h
 *
 *  Created on: 31/07/2016
 *      Author: xavier
 */

#ifndef WIFI_H_
#define WIFI_H_

#include <xil_types.h>

#define WIFI_AP_MAX_LIST 16

typedef enum {
	WIFI_AP_ECN_EMPTY = -1,
	WIFI_AP_ECN_OPEN = 0,
	WIFI_AP_ECN_WEP = 1,
	WIFI_AP_ECN_WPA_PSK = 2,
	WIFI_AP_ECN_WPA2_PSK = 3,
	WIFI_AP_ECN_WPA_WPA2_PSK = 4
} wifi_ap_ecn_t;

typedef char wifi_ap_ssid_t[32];
typedef char wifi_ap_mac_t[32];

typedef int16_t wifi_ap_rssi_t;
typedef u8 wifi_ap_chan_t;
typedef int16_t wifi_ap_fo_t;

typedef enum {
	WIFI_AP_MODE_MANUAL_CONNECTION = 0,
	WIFI_AP_MODE_CONNECTION = 1
} wifi_ap_mode_t;

typedef struct {
	wifi_ap_ecn_t ecn;
	wifi_ap_ssid_t ssid;
	wifi_ap_rssi_t rssi;
	wifi_ap_mac_t mac;
	wifi_ap_chan_t chan;
	wifi_ap_fo_t fo;
} wifi_ap_t;

typedef enum {
	WIFI_FSM_IDLE,
	WIFI_FSM_CWLAP,
	WIFI_FSM_RESET
} wifi_fsm_state_t;

typedef enum {
	WIFI_COMMAND_LIST_ACCESS_POINTS,
	WIFI_COMMAND_NONE,
	WIFI_COMMAND_RESET
} wifi_command_t;

void wifi_init();
void wifi_task();
void wifi_request_lap();
wifi_ap_t* wifi_get_ap_list();
wifi_fsm_state_t wifi_get_fsm_state();
void wifi_request_reset (int argc, char **argv);

#endif /* WIFI_H_ */
