#include "ch.h"
#include "hal.h"
#include <usbcfg.h>

#include <main.h>
#include <obstacle_detection.h>
#include <pi_regulator.h>
#include <motors.h>
#include <leds.h>

#include <move.h>

//set the rotation speed
int16_t set_speed_rot(bool obstacle, int16_t speed_pi){

	int16_t speed = 0;

	if(obstacle)
		speed = 0;
	else
		speed = speed_pi;

	return speed;
}

//set the linear speed
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

        //computes linear speed
        speed = set_speed_lin(obstacle_detection());

        //computes a correction factor to rotate the robot to be on the line
        speed_rotation = set_speed_rot(obstacle_detection(),get_speed_pi());

        //if robot nearly on the line, do not rotate
        if(abs(speed_rotation)<ROTATION_THRESHOLD)
        	speed_rotation = 0;

        //if speed rotation > ROTATION_MAX, limit speed_rotation
        else if(abs(speed_rotation)>ROTATION_MAX){
        	if(speed_rotation<0)
        		speed_rotation = - ROTATION_MAX;
        	else if(speed_rotation>0)
        		speed_rotation = ROTATION_MAX;
        }

        //applies the speed rotation from the PI regulator and the linear speed
		right_motor_set_speed(speed + speed_rotation);
		left_motor_set_speed(speed - speed_rotation);

		//body led ON when rolling
		if(speed == 0 && speed_rotation == 0){
			for(int i=0; i<NUM_RGB_LED; i++)
				toggle_rgb_led(i,RED_LED,RGB_MAX_INTENSITY);

			set_body_led(0);
		}

		//rgb leds ON when robot stops
		if(speed != 0 || speed_rotation != 0){
			clear_leds();
			set_body_led(1);
		}

        //100Hz
        chThdSleepUntilWindowed(time, time + MS2ST(10));
    }
}

void speed_start(void){
	chThdCreateStatic(waSpeed, sizeof(waSpeed), NORMALPRIO, Speed, NULL);
}
