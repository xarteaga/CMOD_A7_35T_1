
#ifndef SW_SYSTEM_PLATFORM_H_H
#define SW_SYSTEM_PLATFORM_H_H

#include "platform_cfg.h"

#ifdef PLATFORM_ENABLE_UART_LOG
#define LOG_OK() xil_printf("[%16s] OK \r\n", __func__)
#define LOG_ERROR() xil_printf("[%16s] ERROR \r\n", __func__)
#define LOG(format,...) xil_printf("[%32s] " format "\r\n", __func__, __VA_ARGS__)
#define PRINT_INIT_OK xil_printf("%32s ... OK\r\n", __func__)
#else
#define LOG_OK() /* Do nothing */
#define LOG_ERROR() /* Do nothing */
#define LOG(format,...) /* Do nothing */
#define PRINT_INIT_OK /* Do nothing */

#endif /* PLATFORM_ENABLE_UART_LOG */

void init_platform(void);
void platform_enable_interrupts(void);

#endif //SW_SYSTEM_PLATFORM_H_H
