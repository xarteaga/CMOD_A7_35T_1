
#ifndef SW_SYSTEM_HAPPYPILLS_HMI_H
#define SW_SYSTEM_HAPPYPILLS_HMI_H

typedef enum {
    HMI_STATE_UNDEFINED = -1,
    HMI_STATE_OFF = 0,
    HMI_STATE_ON = 1,
} hmi_state_t;

void happypills_hmi_init(void);
uint8_t hmi_next_pill (void);

hmi_state_t hmi_get_state (void);

#endif //SW_SYSTEM_HAPPYPILLS_HMI_H
