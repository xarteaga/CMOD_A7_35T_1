
#ifndef _WIFI_UTILS_H
#define _WIFI_UTILS_H

#include "wifi_esp8266.h"

void wifi_utils_parse_cwlap(uint8_t *buf, size_t size, t_wifi_ap_list wifi_ap_list);

#endif /* _WIFI_UTILS_H */
