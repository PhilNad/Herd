#include "Decisions.h"


//Will take care of all decision-making processes most notably when we will need to move through the board.

//Is used to initialize everything at the start of the program
//Is within Decisions because it optimize code-size
void initEverything(){
	//Extern
	initComPort();
	initMotorPins();
	initBoard();
	//Intern
	initAssociatedFunctions();
}

//No operation
void doNothing(){ return; }

//Initialize the functions associated with sensors patterns
void initAssociatedFunctions(){
	for (int i = 0; i <= 16; ++i){
		setAssociatedFunction(i, doNothing);
	}
}