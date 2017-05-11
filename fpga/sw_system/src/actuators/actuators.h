
#ifndef SW_SYSTEM_ACTUATORS_H
#define SW_SYSTEM_ACTUATORS_H

void actuators_init (void);
void actuators_set_motor (dinouts_level_t level);
void actuators_buzzer_turn_on (void);
void actuators_buzzer_turn_off (void);

#endif //SW_SYSTEM_ACTUATORS_H
