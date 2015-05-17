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

MotorAction current_motor_action = MotorAction::STOP;

//Return true is motors are not stopped
bool isMoving(){
	if (current_motor_action != STOP)
		return true;
	return false;
}

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
	current_motor_action = MotorAction::FORWARD;
}

//Speed must be between 0 and 100 percent of full speed
void goForwardRight(int speed){
	int pwm = scaleSpeed(speed);
	digitalWrite(LEFT_MOTOR1, 255);
	analogWrite(LEFT_MOTOR2, pwm);
	analogWrite(RIGHT_MOTOR1, 255);
	analogWrite(RIGHT_MOTOR2, 255);
	current_motor_action = MotorAction::FORWARD_RIGHT;
}

//Speed must be between 0 and 100 percent of full speed
void goForwardLeft(int speed){
	int pwm = scaleSpeed(speed);
	digitalWrite(LEFT_MOTOR1, 255);
	analogWrite(LEFT_MOTOR2, 255);
	analogWrite(RIGHT_MOTOR1, 255);
	analogWrite(RIGHT_MOTOR2, pwm);
	current_motor_action = MotorAction::FORWARD_LEFT;
}

//Speed must be between 0 and 100 percent of full speed
void goBackward(int speed){
	int pwm = scaleSpeed(speed);
	analogWrite(LEFT_MOTOR1,	pwm);
	analogWrite(LEFT_MOTOR2,	255);
	analogWrite(RIGHT_MOTOR1,   pwm);
	analogWrite(RIGHT_MOTOR2,	255);
	current_motor_action = MotorAction::BACKWARD;
}

//Speed must be between 0 and 100 percent of full speed
void goBackwardRight(int speed){
	int pwm = scaleSpeed(speed);
	analogWrite(LEFT_MOTOR1, pwm);
	analogWrite(LEFT_MOTOR2, 255);
	analogWrite(RIGHT_MOTOR1, 255);
	analogWrite(RIGHT_MOTOR2, 255);
	current_motor_action = MotorAction::BACKWARD_RIGHT;
}

//Speed must be between 0 and 100 percent of full speed
void goBackwardLeft(int speed){
	int pwm = scaleSpeed(speed);
	analogWrite(LEFT_MOTOR1, 255);
	analogWrite(LEFT_MOTOR2, 255);
	analogWrite(RIGHT_MOTOR1, pwm);
	analogWrite(RIGHT_MOTOR2, 255);
	current_motor_action = MotorAction::BACKWARD_LEFT;
}

void stopMotors(){
	analogWrite(LEFT_MOTOR1,	255);
	analogWrite(LEFT_MOTOR2,	255);
	analogWrite(RIGHT_MOTOR1,	255);
	analogWrite(RIGHT_MOTOR2,	255);
	current_motor_action = MotorAction::STOP;
}

//Test if the nth bit of data is set
bool is_set(unsigned char data, short n){
	return (data & (1 << n));
}

//Set the nth bit of data
unsigned char clear(unsigned char data, short n){
	data = (data & ~(1 << n));
	return data;
}

//Clear the nth bit of data
unsigned char set(unsigned char data, short n){
	return (data | (1 << n));
}


void stopCondition::addSensorTransition(int nSensor, change c){
	if (c == change::whiteToBlack){
		//Modify bit 0-3, set nSensor bit
		condition = set(condition, nSensor);
	}
	else{
		//Modify bit 4-7
		condition = set(condition, nSensor + 4);
	}
}

void stopCondition::remSensorTransition(int nSensor, change c){
	if (c == change::whiteToBlack){
		//Modify bit 0-3, clear nSensor bit
		condition = clear(condition, nSensor);
	}
	else{
		//Modify bit 4-7
		condition = clear(condition, nSensor + 4);
	}
}

short stopCondition::findPatternToMeetCondition(){
	short old_pattern = calculateSensorPattern();
	short new_pattern = 0;

	for (short i = 0; i < 4; i++){
		//Get all needed bits to computer algorithm
		short old_bit = is_set(old_pattern, i);
		short sc_b2w_bit = is_set(condition, i + 4);
		short sc_w2b_bit = is_set(condition, i);

		//Find needed bit for coherance
		if ((old_bit - sc_b2w_bit + sc_w2b_bit) > 0)
			new_pattern = set(new_pattern, i);
	}

	return new_pattern;
}