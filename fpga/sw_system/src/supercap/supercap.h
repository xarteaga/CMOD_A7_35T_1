
#ifndef SW_SYSTEM_SUPERCAP_H
#define SW_SYSTEM_SUPERCAP_H

#define SUPERCAP_STATE_TOSTR(X) ((const char*)(X==SUPERCAP_STATE_CRITICAL_CHARGE) ?\
                    "critical charge":(X==SUPERCAP_STATE_LOW_CHARGE)? \
                    "low charge":(X==SUPERCAP_STATE_CHARGED)? \
                    "charged":"unkwown")

typedef enum {
    SUPERCAP_STATE_CRITICAL_UNKNOWN = -1,
    SUPERCAP_STATE_CRITICAL_CHARGE = 0,
    SUPERCAP_STATE_LOW_CHARGE = 1,
    SUPERCAP_STATE_CHARGED = 2
} supercap_state_t;

int supercap_init (void);
supercap_state_t supercap_read_status (void);

#endif //SW_SYSTEM_SUPERCAP_H
