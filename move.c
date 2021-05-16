#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>
#include <obstacle_detection.h>
#include <pi_regulator.h>
#include <motors.h>
#include <leds.h>

#include <move.h>

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
    int16_t speed_rot_temp = 0;

    while(1){
        time = chVTGetSystemTime();

        //if speed rotation of PI regulator too high, stores the old value
        speed_rot_temp = speed_rotation;

        speed = set_speed_lin(obstacle_detection());

        if(speed == 0 && speed_rotation == 0){
        	chprintf((BaseSequentialStream*)&SD3, "speed = 0 in\n ");
			for(int i=0; i<NUM_RGB_LED; i++) {
				toggle_rgb_led(i,RED_LED,RGB_MAX_INTENSITY);
        	}
        	set_body_led(0);
        }
		if(speed != 0 || speed_rotation != 0){
			chprintf((BaseSequentialStream*)&SD3, "speed != 0 in\n ");
			clear_leds();
			set_body_led(1);
		}


        //computes a correction factor to rotate the robot to be on the line
        speed_rotation = set_speed_rot(obstacle_detection(),get_speed_pi());

        //if robot nearly on the line, do not rotate
        if(abs(speed_rotation)<ROTATION_THRESHOLD)
        	speed_rotation = 0;
        else if(abs(speed_rotation)>ROTATION_MAX)
        	speed_rotation = speed_rot_temp;

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
