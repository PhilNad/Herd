#ifndef DECISIONS_H
#define DECISIONS_H

#include "Arduino.h"
#include "DynSched.h"
#include "IRSensor.h"
#include "Motion.h"
#include "XBeeDM.h"
#include "Communication.h"
#include "Commands.h"

//A direction uses signs to provide axis and values to provide distances
typedef struct direction{
	int x;
	int y;
	direction(){
		x = 0;
		y = 0;
	};
}Direction;

typedef enum _moving_state{
	WAITING,
	MOVING
}moving_state;

moving_state current_moving_state = WAITING;

void initAssociatedTasks();

void initEverything();

void actionDecision();

#endif //DECISIONS_H