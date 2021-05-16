/*

File    : process_image.h
Author  : Isione Bonvalot
Date    : 6 May 2021
REV 1.0

Functions to capture and process images from the camera PO830D

Adapted from the code of TP4 CamReg
taken at https://moodle.epfl.ch/course/view.php?id=467
*/

#ifndef PROCESS_IMAGE_H
#define PROCESS_IMAGE_H

int16_t get_offset_from_center(void);
void process_image_start(void);

#endif /* PROCESS_IMAGE_H */
