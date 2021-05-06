#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <pi_regulator.h>
#include <process_image.h>

#define NORMAL_SPEED 50; //step/s

//simple PI regulator implementation
int16_t pi_regulator(float error){


}

int16_t get_speed_pi(void){

    int16_t speed_rotation = 0;

    //computes a correction factor to let the robot rotate to be in front of the line
    speed_rotation = pi_regulator(get_offset_from_center());
    return speed_rotation;
}
