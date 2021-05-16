/*

File    : obstacle_detection.h
Author  : Malika In-Albon
Date    : 6 May 2021
REV 1.0

Function to detect obstacle with library proximity_sensors.h
*/

#ifndef OBSTACLE_DETECTION_H_
#define OBSTACLE_DETECTION_H_

//IR0 (front-right)
//IR1 (front-right-45deg)
//IR7 (front-left)
//IR6 (front-left-45deg)

enum{IR0, IR1, IR2, IR3, IR4, IR5, IR6, IR7}; //8 proximity sensors

#define DELTA_MAX 300 //difference between ambient and reflected light

bool obstacle_detection(void);

#endif /* OBSTACLE_DETECTION_H */
