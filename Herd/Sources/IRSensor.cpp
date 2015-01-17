#include "IRSensor.h"


//IR sensors current state
bool isFrontSensorBlack = false;
bool isLeftSensorBlack	= false;
bool isRightSensorBlack = false;
bool isBackSensorBlack  = false;

//IR sensors pins
const int pinFrontSensor	= 13; //IR1
const int pinBackSensor		= 15; //IR3
const int pinLeftSensor		= 16; //IR4
const int pinRightSensor	= 14; //IR2

//Signal values window, used to accurately detect difference between white and black
//Minimum readings
int irMinimum[] = {
	47,  //FRONT_SENSOR
	344, //LEFT_SENSOR
	390, //RIGHT_SENSOR
	675	 //BACK_SENSOR
};
//Maximum readings
int irMaximum[] = {
	1441,//FRONT_SENSOR
	872, //LEFT_SENSOR
	846, //RIGHT_SENSOR
	955	 //BACK_SENSOR
};

//The number of readings we use for the integral. See correctDelimiter();
#define N_READINGS 10

//Readings holder
int readingsMinimum[4][N_READINGS];
int readingMinimumIndex = 0;

//Readings holder
int readingsMaximum[4][N_READINGS];
int readingMaximumIndex = 0;

//IR Sensors patterns holder for comparison purposes
short old_pattern = 0;

//Last checked sensor, used to cycle sensors
int lastRead = BACK_SENSOR;

//Holds functions that are associated with patterns
// function = associatedTask[pattern];
//Initialized by Decisions.cpp
function associatedTask[16];

//Return the sensor that corresponds to the supplied pin. 
//WARNING: Default case is FRONT_SENSOR.
int pinToSensor(int pin){
	switch (pin){
	case pinFrontSensor:
		return FRONT_SENSOR;
	case pinBackSensor:
		return BACK_SENSOR;
	case pinLeftSensor:
		return LEFT_SENSOR;
	case pinRightSensor:
		return RIGHT_SENSOR;
	default:
		return FRONT_SENSOR;
	}
}

//This corrects the delimiter by changing the minimum or maximum values
void correctDelimiter(int sensor, int reading){	
	int currentSensorMin = irMinimum[sensor];
	int currentSensorMax = irMaximum[sensor];

	int delimiter = getDelimiter(sensor);

	//If its the first time we run correctDelimiter();, initialize the defaults values.
	for (int i = 0; i < N_READINGS; i++)
		if (readingsMaximum[sensor][i] == 0)
			readingsMaximum[sensor][i] = irMaximum[sensor];
	for (int i = 0; i < N_READINGS; i++)
		if (readingsMinimum[sensor][i] == 0)
			readingsMinimum[sensor][i] = irMinimum[sensor];

	//Add new data to our integral and then move the corresponding extreme
	if (reading >= delimiter){
		readingsMaximum[sensor][readingMaximumIndex] = reading;
		
		if (readingMaximumIndex == N_READINGS)	readingMaximumIndex = 0;
		else									readingMaximumIndex = readingMaximumIndex + 1;

		int integralResult = 0;
		for (int i = 0; i < N_READINGS; i++)
			integralResult += readingsMaximum[sensor][i];

		//We try a conservative 50% old data, 50% newish data.
		integralResult = integralResult / N_READINGS;
		irMaximum[sensor] = (irMaximum[sensor] + integralResult)/2;
	}
	else{
		readingsMinimum[sensor][readingMinimumIndex] = reading;

		if (readingMinimumIndex == N_READINGS)	readingMinimumIndex = 0;
		else									readingMinimumIndex = readingMinimumIndex + 1;

		int integralResult = 0;
		for (int i = 0; i < N_READINGS; i++)
			integralResult += readingsMinimum[sensor][i];

		//We try a conservative 50% old data, 50% newish data.
		integralResult = integralResult / N_READINGS;
		irMinimum[sensor] = (irMinimum[sensor] + integralResult) / 2;
	}

	// FOR GRAPHING PURPOSES
	/*if (sensor == FRONT_SENSOR){
		DEBUG_SP(reading); DEBUG_SP(","); DEBUG_SP(delimiter); DEBUG_SP(","); DEBUG_SP(irMinimum[sensor]); DEBUG_SP(","); DEBUG_SPL(irMaximum[sensor]);
	}*/

}

//Get the appropriate delimiter for the IR sensor
int getDelimiter(int sensor){
	long temp = (irMaximum[sensor] + irMinimum[sensor]) / 2;
	int delimiter = temp;
	return delimiter;
}

//Simple getter to publicly get current sensor state
bool getSensorState(int sensor){
	switch (sensor){
		case FRONT_SENSOR:
			return isFrontSensorBlack;
		case LEFT_SENSOR:
			return isLeftSensorBlack;
		case RIGHT_SENSOR:
			return isRightSensorBlack;
		case BACK_SENSOR:
			return isBackSensorBlack;
		default:
			return false;
	}
}

//Returns TRUE if the supplied pin number has analog capabilities on the Teensy 3.1 board
bool isAnalogPin(int pinNumber){
	switch (pinNumber){
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
		return true;
	default:
		return false;
	}
}

/*
Return the pattern of the sensors as a short int.
Only the 4 least significant bits are used.
A bit is set if the corresponding sensor is Black

	Sensor		  | Bit
	--------------------
	FRONT_SENSOR	0
	LEFT_SENSOR		1
	RIGHT_SENSOR	2
	BACK_SENSOR		3
*/
short calculateSensorPattern(){
	short result = 0;
	if (isFrontSensorBlack)
		result += 1; //2^0
	if (isLeftSensorBlack)
		result += 2;//2^1
	if (isRightSensorBlack)
		result += 4;//2^2
	if (isRightSensorBlack)
		result += 8;//2^3
	return result;
}

//Add the function as a task associated with the current pattern in the list.
void triggerTaskAssociatedWithPattern(short pattern){
	Task* task = newTask();
	setPriority(task, PRIORITY_HIGH);
	setFunction(task, associatedTask[pattern]);
	addTask(task);
}

//Associate a particular function to a pattern
void setAssociatedFunction(short pattern, function f){
	associatedTask[pattern] = f;
}

//Refresh the sensors pattern and, if needed, request task execution
void refreshSensorsPattern(){
	short sensorsPattern = calculateSensorPattern();
	//If pattern changed, trigger task execution
	if (old_pattern != sensorsPattern)
		triggerTaskAssociatedWithPattern(sensorsPattern);
	old_pattern = sensorsPattern;
}

//This should be registered as a task
void readIRSensor(){

	//Cycle sensor
	int sensorNumber = 0;
	if (lastRead == BACK_SENSOR)	sensorNumber = FRONT_SENSOR;
	else							sensorNumber = lastRead + 1;

	switch (sensorNumber){
	case FRONT_SENSOR:
		if (isAnalogPin(pinFrontSensor))
			isFrontSensorBlack = isOnBlackLineAnalog(pinFrontSensor);
		else
			isFrontSensorBlack = isOnBlackLineDigital(pinFrontSensor);
		lastRead = FRONT_SENSOR;
		break;
	case LEFT_SENSOR:
		if (isAnalogPin(pinLeftSensor))
			isLeftSensorBlack = isOnBlackLineAnalog(pinLeftSensor);
		else
			isLeftSensorBlack = isOnBlackLineDigital(pinLeftSensor);
		lastRead = LEFT_SENSOR;
		break;
	case RIGHT_SENSOR:
		if (isAnalogPin(pinRightSensor))
			isRightSensorBlack = isOnBlackLineAnalog(pinRightSensor);
		else
			isRightSensorBlack = isOnBlackLineDigital(pinRightSensor);
		lastRead = RIGHT_SENSOR;
		break;
	case BACK_SENSOR:
		if (isAnalogPin(pinBackSensor))
			isBackSensorBlack = isOnBlackLineAnalog(pinBackSensor);
		else
			isBackSensorBlack = isOnBlackLineDigital(pinBackSensor);
		lastRead = BACK_SENSOR;
		break;
	//We should never get here but if we do, make sure we dont fuck up. Calling function with an inappropriate pin would be dangerous.
	default:
		break;
	}
	
	refreshSensorsPattern();
}

//This function take roughly 122uS to execute
bool isOnBlackLineAnalog(int sensorPin){
	pinMode(sensorPin, OUTPUT);
	analogWrite(sensorPin, 255);

	delayMicroseconds(10);
	pinMode(sensorPin, INPUT);
	// We need 100 microseconds discharge time to get a reasonably accurate measure
	delayMicroseconds(100);
	int vMeasure = analogRead(sensorPin);

	//Get current value of delimiter
	int sensor = pinToSensor(sensorPin);
	int delimiter = getDelimiter(sensor);

	//Use our current reading to change the delimiter value
	correctDelimiter(sensor, vMeasure);

	if (vMeasure > delimiter)
		return true;//Black
	else
		return false;//White
}

//This function takes between 110uS-3010uS to execute , experimentally took between 300us - 2300us to execute.
bool isOnBlackLineDigital(int sensorPin){
	pinMode(sensorPin, OUTPUT);
	digitalWrite(sensorPin, HIGH);
	//Charge time
	delayMicroseconds(10);
	pinMode(sensorPin, INPUT);

	long time = micros();

	//The maximum discharge time is 3000uS, minimum is around 100uS
	while (digitalRead(sensorPin) == HIGH && micros() - time < 3000);
	int diff = micros() - time;
	
	//Get current value of delimiter
	int sensor = pinToSensor(sensorPin);
	int delimiter = getDelimiter(sensor);

	//Use our current reading to change the delimiter value
	correctDelimiter(sensor, diff);

	if (diff > delimiter)
		return true;//Black
	else
		return false;//White
}