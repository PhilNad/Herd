#include "Arduino.h"

#ifndef MOTIOND_H
#define MOTION_H

//MCU pins connected to channels pins
//TODO change correct pins for v0.2
#define AIN1	3
#define AIN2	6 
#define BIN1	5
#define BIN2	4

//User-friendly names
#define LEFT_MOTOR1 AIN1
#define LEFT_MOTOR2 AIN2
#define RIGHT_MOTOR1 BIN1
#define RIGHT_MOTOR2 BIN2

//PWM bounds determines experimentally
#define MAX_PWM_SPEED 0
#define MIN_PWM_SPEED 220
#define FULL_SCALE abs(MAX_PWM_SPEED-MIN_PWM_SPEED)

int scaleSpeed(int percent);
void initMotorPins(void);

void goForward(int speed);
void goForwardRight(int speed);
void goForwardLeft(int speed);
void goBackward(int speed);
void goBackwardRight(int speed);
void goBackwardLeft(int speed);
void stopMotors();


#endif //MOTION_H