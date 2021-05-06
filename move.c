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


