#ifndef PI_REGULATOR_H
#define PI_REGULATOR_H

//start the PI regulator thread
void pi_regulator_start(void);

int16_t get_speed_pi(void);

#endif /* PI_REGULATOR_H */
