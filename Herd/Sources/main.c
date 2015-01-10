/*
Phil@Uberlab on 20/09/14

README: For an unknown reason, the Teensyduino generated objects files are working properly while the compiled sources create non-working
		object files and thus non-working programs. Finding problems and fixing them is very tedious, for the sake of productivity we will
		use a pre-compiled and linked object file (teensy-coreAll.o)
*/

//WARNING: CONSIDER ALL HEADERS INCLUDED IN THIS PROJECT TO BE MODIFIED SPECIFICALLY FOR THE HERD PROJECT

//The header that will include them all.
#include "Arduino.h"
//Xbee DigiMesh
#include "XbeeDM.h"
//Dynamic Scheduler
#include "DynSched.h"
//Communication Management
#include "Communication.h"
//IR Sensor management
#include "IRSensor.h"
//Motors and motion
#include "Motion.h"
//Everything related to space
#include "Board.h"

//Default tasks
void createDefaultTasks(void){
	//Create a task that will initialize the communication port
	Task* task = newTask();
	setPriority(task, PRIORITY_HIGH);
	setFunction(task, initComPort);
	addTask(task);

	//Create a task that will initialize motors
	task = newTask();
	setPriority(task, PRIORITY_HIGH);
	setFunction(task, initMotorPins);
	addTask(task);

	//Create a task that will initialize the main board
	task = newTask();
	setPriority(task, PRIORITY_HIGH);
	setFunction(task, initBoard);
	addTask(task);

	//Create a permament task that checks if we have Xbee Data ready to be interpreted and shout it back to the terminal.
	task = newTask();
	setPriority(task, PRIORITY_NORMAL);
	setPermanent(task, true);
	setFunction(task, rxComHandler);
	addTask(task);

	//Create a permanent task that checks weather a IR sensor has hit a black line
	task = newTask();
	setPriority(task, PRIORITY_NORMAL);
	setPermanent(task, true);
	setFunction(task, readIRSensor);
	addTask(task);

	//Create a permanent task that check if there is a command we need to interpret
	task = newTask();
	setPriority(task, PRIORITY_NORMAL);
	setPermanent(task, true);
	setFunction(task, pollCommands);
	addTask(task);
}

int main(){
	_init_Teensyduino_internal_();

	createDefaultTasks();

	while (1){
		//Start the Dynamic Scheduler
		iterateNormal();
	}
}