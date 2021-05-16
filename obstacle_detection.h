/*

File    : obstacle_detection.h
Author  : Malika In-Albon
Date    : 6 May 2021
REV 1.0

Function to detect obstacle with library proximity_sensors.h
*/

#ifndef OBSTACLE_DETECTION_H_
#define OBSTACLE_DETECTION_H_

#define DELTA_MAX 300 //difference between ambient and reflected light

bool obstacle_detection(void);

#endif /* OBSTACLE_DETECTION_H */
