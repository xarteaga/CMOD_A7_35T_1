//
// Created by vaxi on 5/2/17.
//

#ifndef SW_SYSTEM_LEDS_H
#define SW_SYSTEM_LEDS_H

#define LEDS_COUNT 5

typedef enum {
    LEDS_CHAN_ORANGE_1 = 0,
    LEDS_CHAN_ORANGE_2 = 1,
    LEDS_CHAN_RED = 2,
    LEDS_CHAN_GREEN = 3,
    LEDS_CHAN_BLUE = 4,
} leds_chan_t;

int leds_init (void);
void leds_turn_on (leds_chan_t chan);
void leds_turn_off (leds_chan_t chan);

#endif //SW_SYSTEM_LEDS_H
