#include "ch.h"
#include "hal.h"

#include <sensors/proximity.h>

#include <obstacle_detection.h>

//IR0 (front-right)
//IR1 (front-right-45deg)
//IR7 (front-left)
//IR6 (front-left-45deg)

enum{IR0, IR1, IR2, IR3, IR4, IR5, IR6, IR7}; //8 proximity sensors

bool obstacle_detection(void){

	uint8_t i = 0;
	bool obstacle = 0;
	int16_t delta[PROXIMITY_NB_CHANNELS] = {0};

	//get delta of all proximity sensors
	for(i=0; i<PROXIMITY_NB_CHANNELS; i++){
		delta[i] = get_prox(i);
		if (delta[IR0] > DELTA_MAX || delta[IR7] > DELTA_MAX)
			obstacle = 1;
	}

	return obstacle;
}

