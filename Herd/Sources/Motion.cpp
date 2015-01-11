 #include "Arduino.h"
#include "Motion.h"

/*
	IN1		IN2		FUNCTION
	1		PWM		Forward, PWM being the speed
	PWM		1		Backward, PWM being the speed
	1		1		Brake

	Channel A = LEFT MOTOR
	Channel B = RIGHT MOTOR

	MOTOR_DRIVER_INPUT		MCU_PIN		definition
	AIN1					2			LEFT_MOTOR1
	AIN2					3			LEFT_MOTOR2
	BIN1					5			RIGHT_MOTOR1
	BIN2					4			RIGHT_MOTOR2

	Summary:
	pin2	pin3	pin5	pin4	Action
	1		PWM		1		PWM		Forward
	PWM		1		PWM		1		Backward
	1		1		1		1		Brake
	1		PWM		1		1		Forward right
	1		1		1		PWM		Forward left
	PWM		1		1		1		Backward right
	1		1		PWM		1		Backward left
*/

//Return the PWM associated with a particular percentage of full speed.
//0 means stop. 100 means full speed
int scaleSpeed(int percent){
	//Check bounds
	if (percent > 100 || percent <= 0)
		return 255;

	float speed = FULL_SCALE - (percent*FULL_SCALE)/100;

	return (int)speed;
}

void initMotorPins(void){
	
	//Temporary short between pin 6 (PWM capable) and pin 2 (PWM non-capable)
	//Fixed in v0.2 TOFIX
	pinMode(2, INPUT);

	pinMode(LEFT_MOTOR1, OUTPUT);
	pinMode(LEFT_MOTOR2, OUTPUT);
	pinMode(RIGHT_MOTOR1, OUTPUT);
	pinMode(RIGHT_MOTOR2, OUTPUT);
	stopMotors();
}

//Speed must be between 0 and 100 percent of full speed
void goForward(int speed){
	int pwm = scaleSpeed(speed);
	digitalWrite(LEFT_MOTOR1,	255);
	analogWrite(LEFT_MOTOR2,	pwm);
	analogWrite(RIGHT_MOTOR1,	255);
	analogWrite(RIGHT_MOTOR2,	pwm);
}

//Speed must be between 0 and 100 percent of full speed
void goForwardRight(int speed){
	int pwm = scaleSpeed(speed);
	digitalWrite(LEFT_MOTOR1, 255);
	analogWrite(LEFT_MOTOR2, pwm);
	analogWrite(RIGHT_MOTOR1, 255);
	analogWrite(RIGHT_MOTOR2, 255);
}

//Speed must be between 0 and 100 percent of full speed
void goForwardLeft(int speed){
	int pwm = scaleSpeed(speed);
	digitalWrite(LEFT_MOTOR1, 255);
	analogWrite(LEFT_MOTOR2, 255);
	analogWrite(RIGHT_MOTOR1, 255);
	analogWrite(RIGHT_MOTOR2, pwm);
}

//Speed must be between 0 and 100 percent of full speed
void goBackward(int speed){
	int pwm = scaleSpeed(speed);
	analogWrite(LEFT_MOTOR1,	pwm);
	analogWrite(LEFT_MOTOR2,	255);
	analogWrite(RIGHT_MOTOR1,   pwm);
	analogWrite(RIGHT_MOTOR2,	255);
}

//Speed must be between 0 and 100 percent of full speed
void goBackwardRight(int speed){
	int pwm = scaleSpeed(speed);
	analogWrite(LEFT_MOTOR1, pwm);
	analogWrite(LEFT_MOTOR2, 255);
	analogWrite(RIGHT_MOTOR1, 255);
	analogWrite(RIGHT_MOTOR2, 255);
}

//Speed must be between 0 and 100 percent of full speed
void goBackwardLeft(int speed){
	int pwm = scaleSpeed(speed);
	analogWrite(LEFT_MOTOR1, 255);
	analogWrite(LEFT_MOTOR2, 255);
	analogWrite(RIGHT_MOTOR1, pwm);
	analogWrite(RIGHT_MOTOR2, 255);
}

void stopMotors(){
	analogWrite(LEFT_MOTOR1,	255);
	analogWrite(LEFT_MOTOR2,	255);
	analogWrite(RIGHT_MOTOR1,	255);
	analogWrite(RIGHT_MOTOR2,	255);
}