
#ifndef SW_SYSTEM_XADC_H
#define SW_SYSTEM_XADC_H


#include "xil_types.h"

typedef enum {
    XADC_CHANNEL_0 = 0,
    XADC_CHANNEL_1 = 1,
    XADC_CHANNEL_2 = 2,
    XADC_CHANNEL_3 = 3,
    XADC_CHANNEL_UNDEFINED = 5,
} xadc_channel_t;

int xadc_init (void);
uint16_t xadc_read_raw(xadc_channel_t channel);
uint16_t xadc_read_mv(xadc_channel_t channel);

#endif //SW_SYSTEM_XADC_H
