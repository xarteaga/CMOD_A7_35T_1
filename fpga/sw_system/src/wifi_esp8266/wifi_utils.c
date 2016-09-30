
/* Standard C includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* BSP & Xilinx includes */
#include "xil_types.h"

/* Project includes */
#include "wifi_utils.h"

/* Defines */
#define WIFI_UTILS_CWLAP_ENTRY_NUM_ATTR 8

void wifi_utils_parse_cwlap_entry(uint8_t *buf, size_t size, t_wifi_ap *wifi_ap){
    uint8_t *params[WIFI_UTILS_CWLAP_ENTRY_NUM_ATTR];
    size_t count = 0, k = 0;

    /* Set first param */
        params[count] = &buf[k];
        count ++;

        /* Find paramaters delimitation */
        for (k = 0; (k < size) && (count < WIFI_UTILS_CWLAP_ENTRY_NUM_ATTR); k++) {
            if (buf[k] == ','){
                /* Set end of previous param */
                buf[k] = '\0';

                /* Set next param end */
                params[count] = &buf[k + 1];

                /* Increase count */
                count ++;
            }
        }

    /* set last parameter end */
    buf[k] = '\0';

    /* Parse ECN */
    wifi_ap->ecn = (t_wifi_ap_ecn) atoi((char*)params[0]);

    /* Parse SSID */
    strncpy((char *) wifi_ap->ssid, (char*)(params[1] + 1), WIFI_AP_SSID_MAXLEN);
    wifi_ap->ssid[strlen((char*)params[1]) - 2] = '\0';

    /* Parse RSSI */
    wifi_ap->rssi = atoi((char*)params[2]);

    /* Print trace */
    //xil_printf("[%s] %d %16s %d\r\n", __FUNCTION__, wifi_ap->ecn, wifi_ap->ssid, wifi_ap->rssi);
}


size_t wifi_utils_parse_cwlap(uint8_t *buf, size_t size, t_wifi_ap_list *wifi_ap_list) {
    u8 head[] = "+CWLAP:";
    size_t k, start, ap_count = 0;

    /* For each character ...*/
    for (k = 0; (k < (size - sizeof(head))) && (ap_count < WIFI_AP_MAXNUM); k++) {
        /* ... Check if it matches ... */
        if (strncmp((char *) &buf[k], (char *) head, sizeof(head) - 1) == 0) {
            /* ... stores start index ... */
            start = k + sizeof(head);

            /* ... finds end ... */
            while ((k < (size - sizeof(head))) && (buf[k] != '\r')) {
                k++;
            }
            k--;

            if (k != (size - sizeof(head))) {
                /* Copy */
                memcpy((char *) wifi_ap_list->entries[ap_count].ssid, (char *) &buf[start], (size_t) (k - start));

                /* Parse line */
                wifi_utils_parse_cwlap_entry(&buf[start], (size_t) (k - start), &wifi_ap_list->entries[ap_count]);

                /* Increase count */
                ap_count++;
            }
        }

    }

    /* Update AP count */
    wifi_ap_list->count = ap_count;

    /* Return number of detected APs */
    return ap_count;
}
