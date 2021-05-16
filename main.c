#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "spi_comm.h"
#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>
#include <motors.h>
#include <camera/po8030.h>
#include <sensors/proximity.h>
#include <leds.h>

#include <pi_regulator.h>
#include <process_image.h>
#include <move.h>

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}

int main(void)
{
    halInit();
    chSysInit();
    mpu_init();

    //inits the Inter Process Communication bus
    messagebus_init(&bus, &bus_lock, &bus_condvar);

    //inits leds
    clear_leds();
    set_body_led(0);

    //starts the camera
    dcmi_start();
	po8030_start();

    //starts the proximity sensors and the thread proximity
    proximity_start();

	//inits the motors
	motors_init();

	//start the spi communicatoin
	spi_comm_start();

	//starts the serial communication
	serial_start();

	//starts the threads for the speed of the robot 
	speed_start();

	//starts the processing of the image
	process_image_start();

    /* Infinite loop. */
    while (1) {
    	//waits 1 second
        chThdSleepMilliseconds(1000);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
