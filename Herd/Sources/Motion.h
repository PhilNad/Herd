#ifndef MOTION_H
#define MOTION_H

#include "Arduino.h"
#include "IRSensor.h"

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

typedef enum _motor_action{
		FORWARD,
		FORWARD_RIGHT,
		FORWARD_LEFT,
		BACKWARD,
		BACKWARD_RIGHT,
		BACKWARD_LEFT,
		STOP
}MotorAction;

bool isMoving();

int scaleSpeed(int percent);
void initMotorPins(void);

void goForward(int speed);
void goForwardRight(int speed);
void goForwardLeft(int speed);
void goBackward(int speed);
void goBackwardRight(int speed);
void goBackwardLeft(int speed);
void stopMotors();

class stopCondition{
	unsigned char condition;
public:
	stopCondition() : condition(0){};
	enum change{ whiteToBlack, blackToWhite };

	//Add a sensor transition to the condition
	void addSensorTransition(int nSensor, change c);

	//Remove a sensor transition to the condition
	void remSensorTransition(int nSensor, change c);

	//Given the last sensor pattern,
	//Find the pattern needed so the stop condition is met
	short findPatternToMeetCondition();
};


#endif //MOTION_H