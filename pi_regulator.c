#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <pi_regulator.h>
#include <process_image.h>

//simple PI regulator implementation
int16_t pi_regulator(int16_t error){

    float speed = 0;

    static int32_t sum_error = 0;

    //disables the PI regulator if the error is to small
    //this avoids to always move as we cannot exactly be where we want and 
    //the camera is a bit noisy
    if(fabs(error) < ERROR_THRESHOLD){
        return 0;
    }

    sum_error += error;

    //we set a maximum and a minimum for the sum to avoid an uncontrolled growth
    if(sum_error > MAX_SUM_ERROR){
        sum_error = MAX_SUM_ERROR;
    }else if(sum_error < -MAX_SUM_ERROR){
        sum_error = -MAX_SUM_ERROR;
    }

    speed = KP * error + KI * sum_error;
    //chprintf((BaseSequentialStream *)&SD3, "error = %f \n",error);

    return (int16_t)speed;


}

int16_t get_speed_pi(void){

    int16_t speed_rotation = 0;

    //computes a correction factor to let the robot rotate to be in front of the line
    speed_rotation = pi_regulator(get_offset_from_center());
    return speed_rotation;
}
