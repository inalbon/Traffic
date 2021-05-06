#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <sensors/proximity.h>

//IR0 (front-right)
//IR1 (front-right-45deg)
//IR7 (front-left)
//IR6 (front-left-45deg)
#define IR0 0, IR1 1, IR2 2, IR3 3, IR4 4, IR5 5, IR6 6, IR7 7
#define DELTA_MAX 300 //difference between ambient and reflected light

bool obstacle_detection(void){
	uint16_t i = 0;
	bool obstacle = 0;
	int16_t delta[PROXIMITY_NB_CHANNELS] = {0};

	for(i=0; i<PROXIMITY_NB_CHANNELS; i++){
		delta[i] = get_prox(i);
		chprintf((BaseSequentialStream*)&SD3, "\n delta %d = %d \t ", i, delta[i]);
		if (delta[i] > DELTA_MAX)
			obstacle = 1;
	}

	chprintf((BaseSequentialStream*)&SD3, "\n");


	return obstacle;
}

