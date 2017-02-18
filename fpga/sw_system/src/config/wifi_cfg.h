
#ifndef WIFI_CFG_H_
#define WIFI_CFG_H_

#define WIFI_CFG_SSID 	"JAZZTEL_01"
#define WIFI_CFG_PWD 	"E64680C076E01"

/*
 * Buffered memory
 */
#define BUFFER_MAX_MEMORY               256*1024

/*
 * HTTP Client
 */
#define HTTP_CLIENT_SEND_BUFFER_SIZE    4096
#define HTTP_CLIENT_RECV_BUFFER_SIZE    4096
#define HTTP_CLIENT_SERVER_ADDR         "www.google.com"
#define HTTP_CLIENT_SERVER_PORT         80

/*
 * WiFi ESP8266
 */
#define WIFI_ESP8266_MAX_MSG_LEN        1500
#define WIFI_AP_SSID_MAXLEN 16
#define WIFI_AP_MAXNUM      16

/*
 * DEBUG TRACES
 */
#define BUFFER_DEBUG_TRACES             0
#define ESP8266_DEBUG_TRACES            1
#define OPENAT_DEBUG_TRACES             0

#endif /* WIFI_CFG_H_ */
