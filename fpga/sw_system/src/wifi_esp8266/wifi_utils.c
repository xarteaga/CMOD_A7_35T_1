
/* Standard C includes */
#include <string.h>
#include <stdio.h>

/* BSP & Xilinx includes */
#include "xil_types.h"

/* Project includes */
#include "wifi_utils.h"

void wifi_utils_parse_cwlap(uint8_t *buf, size_t size, t_wifi_ap_list wifi_ap_list) {
    u8 head[] = "+CWLAP:";
    size_t k, start, ap_count = 0;

    //xil_printf("%d %d\r\n%s", size, (size - sizeof(head)), buf);


    /* For each character ...*/
    for (k = 0; k < (size - sizeof(head)); k++) {
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
                memcpy((char *) wifi_ap_list[ap_count].ssid, (char *) &buf[start], (size_t) (k - start));

                /* Debug trace */
                xil_printf("[%s] %s\r\n", __FUNCTION__, (char *) wifi_ap_list[ap_count].ssid);
            }
        }

    }


}
