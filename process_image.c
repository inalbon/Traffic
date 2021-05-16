#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>

#include <process_image.h>

static int16_t offset_from_center = 0;

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);

/*
 *  Returns the offset between the center of the line and
 *	the center of the captured image
 */
int16_t extract_offset_from_center(uint8_t *buffer){
	
	int16_t offset = 0;
	uint16_t i = 0, begin = 0, end = 0;
	uint8_t stop = 0, wrong_line = 0, line_not_found = 0;
	uint32_t mean = 0;

	//performs an average
	for(uint16_t i = 0 ; i < IMAGE_BUFFER_SIZE ; i++)
		mean += buffer[i];

	mean /= IMAGE_BUFFER_SIZE;

	do{
		wrong_line = 0;
		//search for a begin
		while(stop == 0 && i < (IMAGE_BUFFER_SIZE - WIDTH_SLOPE)){ 
			//the slope must at least be WIDTH_SLOPE wide and is compared
		    //to the mean of the image
		    if(buffer[i] > mean && buffer[i+WIDTH_SLOPE] < mean){
		        begin = i;
		        stop = 1;
		    }
		    i++;
		}
		//if a begin was found, search for an end
		if (i < (IMAGE_BUFFER_SIZE - WIDTH_SLOPE) && begin){
		    stop = 0;
		    
		    while(stop == 0 && i < IMAGE_BUFFER_SIZE){
		        if(buffer[i] > mean && buffer[i-WIDTH_SLOPE] < mean){
		            end = i;
		            stop = 1;
		        }
		        i++;
		    }
		    //if an end was not found
		    if (i > IMAGE_BUFFER_SIZE || !end)
		        line_not_found = 1;
		}
		else//if no begin was found
		    line_not_found = 1;

		//if a line too small has been detected, continues the search
		if(!line_not_found && (end-begin) < MIN_LINE_WIDTH){
			i = end;
			begin = 0;
			end = 0;
			stop = 0;
			wrong_line = 1;
		}
	}while(wrong_line);

	// if line not found, take last offset sign into account to decide which side to turn
	if(line_not_found){
		if(offset_from_center > 0)
			offset = OFFSET_MAX;
		else if(offset_from_center < 0)
			offset = -OFFSET_MAX;
	}
	else
		offset = (IMAGE_BUFFER_SIZE-end-begin)/2;

	//chprintf((BaseSequentialStream*)&SD3, "\n offset =  %d \n ", offset);

	return offset;
}


static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	//Takes pixels 0 to IMAGE_BUFFER_SIZE of the bottom lines 479 + 480 (minimum 2 lines because reasons)
	po8030_advanced_config(FORMAT_RGB565, 0, 479, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);
	dcmi_enable_double_buffering();
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	dcmi_prepare();

    while(1){
        //starts a capture
		dcmi_capture_start();
		//waits for the capture to be done
		wait_image_ready();
		//signals an image has been captured
		chBSemSignal(&image_ready_sem);
    }
}


static THD_WORKING_AREA(waProcessImage, 1024);
static THD_FUNCTION(ProcessImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	uint8_t *img_buff_ptr;
	uint8_t image[IMAGE_BUFFER_SIZE] = {0};

	bool send_to_computer = true;

    while(1){
    	//waits until an image has been captured
        chBSemWait(&image_ready_sem);
		//gets the pointer to the array filled with the last image in RGB565    
		img_buff_ptr = dcmi_get_last_image_ptr();

		//Extracts only the red pixels
		for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2)
			image[i/2] = (uint8_t)img_buff_ptr[i]&0xF8; //extracts first 5bits of the first byte
													   //takes nothing from the second byte

		//search for an offset between the center of the line to follow and the center of the image
		offset_from_center = extract_offset_from_center(image);

		if(send_to_computer)
			SendUint8ToComputer(image, IMAGE_BUFFER_SIZE);//sends to the computer the image

		//invert the bool
		send_to_computer = !send_to_computer;
    }
}

int16_t get_offset_from_center(void){
	return offset_from_center;
}

void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
}
