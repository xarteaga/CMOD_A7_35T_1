/*
 * wifi_utils.c
 *
 *  Created on: 11/08/2016
 *      Author: xavier
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "xil_types.h"

#include "../wifi.h"

/*HELP: https://cdn.sparkfun.com/assets/learn_tutorials/4/0/3/4A-ESP8266__AT_Instruction_Set__EN_v0.30.pdf*/

char *  wifi_utils_strfind (char *a, char *b) {
	char *a_back = a, *b_back = b, *ret = 0;

	//xil_printf("a_back: %8X a: %8X b_back: %8X b: %8X ret : %8X\r\n", a_back, a, b_back, b, ret);
	while(*a_back != 0 && ret == 0) {
		// Restores second string pointer
		a = a_back;
		b = b_back;

		while(*a == *b && *a != 0 && *b != 0) {
				++a;
				++b;
		}

		if (*b == 0) {
			ret = a_back;
			xil_printf("a_back: %8X a: %8X b_back: %8X b: %8X ret : %8X\r\n", a_back, a, b_back, b, ret);
			//xil_printf("MATCHED!!\r\n");

		}

		++a_back;
	}

	return ret;
}

u8 wifi_utils_parse_ap (char *buf, wifi_ap_t *wifi_ap_list) {
	u32 n = 0;
	char *params[5];

	/* Split buffer */
	while (buf != 0){
		// Store WiFi parameter pointer and increase index
		params[n] = buf;
		++n;

		// Find the end of the parameter with ',' delimiter
		buf = wifi_utils_strfind(buf, ",");

		// If end of parameter found
		if (buf != 0){
			// Write end of parameter string
			*buf = 0;
			++buf;
		}
	}

	/* PARSE ECN */
	xil_printf("       ECN: %s\r\n", params[0]);
	wifi_ap_list->ecn = atoi(params[0]);

	/* PARSE SSID */
	memset(wifi_ap_list->ssid, 0, 32);
	memcpy(wifi_ap_list->ssid, params[1] + 1, strlen(params[1]) - 2);
	xil_printf("      SSID: %s\r\n", wifi_ap_list->ssid);

	/* PARSE RSSI */
	xil_printf("      RSSI: %s\r\n", params[2]);
	wifi_ap_list->rssi = atoi(params[2]);

	/* PARSE MAC */
	memset(wifi_ap_list->mac, 0, 32);
	memcpy(wifi_ap_list->mac, params[3] + 1, strlen(params[3]) - 2);
	xil_printf("       MAC: %s\r\n", wifi_ap_list->mac);

	/* PARSE CHANNEL */
	xil_printf("   Channel: %s\r\n", params[4]);
	wifi_ap_list->chan = atoi(params[4]);

	/* PARSE Frequency offset */
	xil_printf("Freq. Off.: %s\r\n", params[5]);
	wifi_ap_list->fo = atoi(params[5]);

	return n;

}

u8 wifi_utils_parse_cwlap (char *buf, wifi_ap_t *wifi_ap_list) {
	char* buf_offset = buf;
	char *begin_ptr = buf, *end_ptr = buf;
	u32 n = 0;

	 while (buf != 0){
		// Scan buffer for pattern
		begin_ptr = wifi_utils_strfind(buf, "+CWLAP:(");
		end_ptr = wifi_utils_strfind(begin_ptr, ")");

		// If pattern found...
		if (begin_ptr != 0 && end_ptr != 0) {
			// Advance begin pointer
			begin_ptr = begin_ptr + 8;

			// Close string
			*end_ptr = 0;

			xil_printf("[WIFI AP] %s\r\n", begin_ptr);
			wifi_utils_parse_ap(begin_ptr, &(wifi_ap_list[n]));
			++n;

			// Update initial buffer pointer
			buf = end_ptr + 1;
		} else {
			// Set exit condition
			buf = 0;
		}
	}

	 wifi_ap_list[n].ecn = WIFI_AP_ECN_EMPTY;

	return n;
}
