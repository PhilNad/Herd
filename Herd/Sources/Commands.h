#ifndef COMMANDS_H
#define COMMANDS_H

#include "Arduino.h"
#include "Board.h"
#include "DynSched.h"
#include "Communication.h"


//POS command
void broadcastPositionCommand(Rx_data data);
//BRC command
void demandPositionCommand();
//NEW command
void newTargetCommand(Rx_data data);

#endif //COMMANDS_H