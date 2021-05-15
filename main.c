#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "spi_comm.h"
#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <motors.h>
#include <camera/po8030.h>
#include <sensors/proximity.h>
#include <leds.h>
#include <audio/play_melody.h>
#include <chprintf.h>

#include <pi_regulator.h>
#include <process_image.h>
#include <move.h>

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

void SendUint8ToComputer(uint8_t* data, uint16_t size) 
{
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)"START", 5);
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)&size, sizeof(uint16_t));
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)data, size);
}

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

    //Inits the Inter Process Communication bus
    messagebus_init(&bus, &bus_lock, &bus_condvar);

    //Init leds
    clear_leds();
    set_body_led(0);



    //start the USB communication
    usb_start();
    //starts the camera
    dcmi_start();
	po8030_start();
    //starts the proximity sensors and the thread proximity
    proximity_start();
	//inits the motors
	motors_init();

	//starts the spi communication
	spi_comm_start();

	//starts the serial communication
	serial_start();

	//stars the threads for the speed of the robot and the processing of the image
	speed_start();
	process_image_start();

	//starts the thread of the melody
	playMelodyStart();

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
