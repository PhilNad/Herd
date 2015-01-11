#include "Arduino.h"
//Xbee Digimesh API mode
#include "XBeeDM.h"

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

//These definitions allows us to write debug and release code without any overhead
#ifdef DEBUG_VERBOSE

#define DEBUG_SP(x)		Serial.print(x)
#define DEBUG_SPL(x)	Serial.println(x)

#else 

#define DEBUG_SP(x)		[]{}
#define DEBUG_SPL(x)	[]{}

#endif

//Maximum number of nodes in the list
#define RX_DATA_LIST_MAXIMUM 100

//Structure defining a node containing information about received data.
typedef struct rx_data{
	uint64_t macAddr;
	uint8_t* payload;
	uint8_t	 payloadSize;
} Rx_data;

//Communication functions
void	initComPort();
void	sendData(uint64_t address, uint8_t payload[], uint8_t payLoadLen);
void	sendDataBroadcast(uint8_t payload[], uint8_t payLoadLen);
void	comGetPacket();
void	dispatchCommands();
void	pollCommands();

//List management functions
void	addRxData(Rx_data rd);
Rx_data getFirstRxData();
void	removeFirstRxData();
boolean isRxDataListEmpty();
void	rxDataListDefrag();

//Debugging and printing functions
void printLLNumber(unsigned long long n, uint8_t base);

#endif //COMMUNICATION_H