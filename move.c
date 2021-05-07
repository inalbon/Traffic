#include "ch.h"
#include "hal.h"
#include <math.h>
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>
#include <obstacle_detection.h>
#include <pi_regulator.h>
#include <motors.h>

#define NORMAL_SPEED 350; // step/s
#define SPEED_MAX 350; //step/s

int16_t set_speed_rot(bool obstacle, int16_t speed_pi){
	int16_t speed = 0;

	if(obstacle)
		speed = 0;
	else
		speed = speed_pi;

	return speed;
}

int16_t set_speed_lin(bool obstacle){
	int16_t speed = 0;
	if(obstacle)
			speed = 0;
	else
		speed = NORMAL_SPEED;

	return speed;
}

static THD_WORKING_AREA(waSpeed, 256);
static THD_FUNCTION(Speed, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    systime_t time;

    int16_t speed = 0;
    int16_t speed_rotation = 0;

    while(1){
        time = chVTGetSystemTime();

        speed = set_speed_lin(obstacle_detection());

        //computes a correction factor to rotate the robot to be on the line
        speed_rotation = set_speed_rot(obstacle_detection(),get_speed_pi());

        //applies the speed rotation from the PI regulator and the linear speed
		right_motor_set_speed(speed + speed_rotation);
		left_motor_set_speed(speed - speed_rotation);

        //100Hz
        chThdSleepUntilWindowed(time, time + MS2ST(10));
    }
}

void speed_start(void){
	chThdCreateStatic(waSpeed, sizeof(waSpeed), NORMALPRIO, Speed, NULL);
}