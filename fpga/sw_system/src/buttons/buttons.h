
#ifndef SW_SYSTEM_BUTTONS_H
#define SW_SYSTEM_BUTTONS_H

#include <buttons_cfg.h>

typedef enum {
    BUTTONS_1 = (int) PLATFORM_BUTTONS_PIN_BUTTON_1,
    BUTTONS_2 = PLATFORM_BUTTONS_PIN_BUTTON_2,
    BUTTONS_3 = PLATFORM_BUTTONS_PIN_BUTTON_3,
} buttons_t;

void buttons_init (void);
dinouts_level_t buttons_read (buttons_t button);

#endif //SW_SYSTEM_BUTTONS_H
