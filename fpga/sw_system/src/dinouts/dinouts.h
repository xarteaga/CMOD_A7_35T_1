
#ifndef SW_SYSTEM_DINOUTS_H
#define SW_SYSTEM_DINOUTS_H

typedef enum {
    DINOUTS_PIN_1 = 0,
    DINOUTS_PIN_2 = 1,
    DINOUTS_PIN_3 = 2,
    DINOUTS_PIN_4 = 3,
    DINOUTS_PIN_5 = 4,
    DINOUTS_PIN_6 = 5,
    DINOUTS_PIN_7 = 6,
    DINOUTS_PIN_8 = 7,
    DINOUTS_PIN_9 = 8,
    DINOUTS_PIN_10 = 9,
    DINOUTS_PIN_11 = 10,
    DINOUTS_PIN_12 = 11,
    DINOUTS_PIN_13 = 12,
    DINOUTS_PIN_14 = 13,
    DINOUTS_PIN_15 = 14,
    DINOUTS_PIN_16 = 15,
    DINOUTS_PIN_UNKNOWN = 16
} dinouts_pin_t;

typedef enum {
    DINOUTS_DIRECTION_UNDEFINED = 0,
    DINOUTS_DIRECTION_IN = 1,
    DINOUTS_DIRECTION_OUT = 2,
} dinouts_direction_t;

typedef enum {
    DINOUTS_UNDEFINED = -1,
    DINOUTS_LOW = 0,
    DINOUTS_HIGH = 1
} dinouts_level_t;

#define DINOUT_LEVEL_STR(X) (const char *)((X==DINOUTS_LOW)?"LOW":(X==DINOUTS_HIGH)?"HIGH":"undef")

int dinouts_init(void);
void dinouts_set_direction (dinouts_pin_t chan, dinouts_direction_t direction);
void dinouts_turn_on (dinouts_pin_t chan);
void dinouts_turn_off (dinouts_pin_t chan);
dinouts_level_t dinouts_read (dinouts_pin_t chan);

#endif //SW_SYSTEM_DINOUTS_H
