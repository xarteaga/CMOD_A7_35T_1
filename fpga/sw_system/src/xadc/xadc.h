
#ifndef SW_SYSTEM_XADC_H
#define SW_SYSTEM_XADC_H


#include "xbasic_types.h"

typedef enum {
    XADC_CHANNEL_0,
    XADC_CHANNEL_1,
} xadc_channel_t;

int xadc_init (void);
u16 xadc_read_raw(xadc_channel_t channel);

#endif //SW_SYSTEM_XADC_H
