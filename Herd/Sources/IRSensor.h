#include "Arduino.h"

#ifndef IRSENSOR_H
#define IRSENSOR_H

/* TO READ
IR sensors should be on those pins:
IR1: A0 - pin 14
IR2: A1 - pin 15
IR3: A2 - pin 16
IR4: A3 - pin 17

But unfortunately on v0.1 board, we use these pins:
IR1: 13 - pin 13
IR2: A0 - pin 14
IR3: A1 - pin 15
IR4: A2 - pin 16

Because of the pin 13 which is digital, we need to adapt our code to support digital reading of IR sensors.
Digital reading has two main disavantages:
	* It is less accurate
	* It takes roughly 10 times the execution time of the analog reading

We should change this on a potential v0.2
Also, make sure that the Analog pin used for reading can be Input AND Output AND is +5v capable (leaving us with A0-A9).
We should use the analog sensors (https://www.sparkfun.com/products/9453) instead of the digital ones, also, we should put
a op-amp that transform our +5V output into +3.3v output. This would do the job for our 4 sensors (https://www.sparkfun.com/products/12009)
If we design our own sensor board we could actually add interrupt capability.
*/

#define FRONT_SENSOR	0
#define LEFT_SENSOR		1
#define RIGHT_SENSOR	2
#define BACK_SENSOR		3

int getDelimiter(int sensor);

bool getSensorState(int sensor);

bool isAnalogPin(int pinNumber);

void readIRSensor();

bool isOnBlackLineAnalog(int sensorPin);

bool isOnBlackLineDigital(int sensorPin);

#endif //IRSENSOR_H