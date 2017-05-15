
#ifndef SW_SYSTEM_HAPPYPILLS_CONTROLLER_H
#define SW_SYSTEM_HAPPYPILLS_CONTROLLER_H

typedef enum {
    CONTROLLER_STATE_UNDEFINIED = 0,
    CONTROLLER_STATE_ROTATING = 1,
    CONTROLLER_STATE_WAITING = 2,
    CONTROLLER_STATE_SYNCH = 3,
    CONTROLLER_STATE_IDLE = 4,
} controller_state_t;

void happypills_controller_init(void);
controller_state_t controller_get_state(void);

#endif //SW_SYSTEM_HAPPYPILLS_CONTROLLER_H
