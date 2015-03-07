#ifndef COMMANDS_H
#define COMMANDS_H

#include "Arduino.h"
#include "Board.h"
#include "DynSched.h"
#include "Communication.h"


//POS command
void broadcastPositionCommand();
//BRC command
//TODO: Change name
void demandPositionCommand();
//NEW command
void newTargetCommand();

#endif //COMMANDS_H