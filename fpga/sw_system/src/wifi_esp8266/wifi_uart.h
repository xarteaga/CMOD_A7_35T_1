
#ifndef WIFI_UART_H_
#define WIFI_UART_H_

/* BSP & Xilinx Includes */
#include "xil_types.h"

/* Routines prototypes */
void wifi_uart_init(void);
void wifi_uart_write(u8 *buf, u32 size);
size_t wifi_uart_read_key(uint8_t *buf, size_t maxsize, const uint8_t *key);
u32 wifi_uart_read_ERROR(char *buf, u32 maxsize);
void wifi_uart_task(void);
u32 wifi_uart_available(void);
u32 wifi_uart_read(u8 *buf, u32 maxsize);

#endif /* WIFI_UART_H_ */
