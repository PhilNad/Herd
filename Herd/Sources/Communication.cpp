#include "Communication.h"

//Xbee DigiMesh library object
XBee xb;

//We limit the list to 100 entries because we are memory limited. We should not have to increase this limit anyways if we are efficient.
Rx_data RxDataList[RX_DATA_LIST_MAXIMUM];

//Points to the head and tail nodes of the list
uint8_t RxDataListHead;
uint8_t RxDataListTail;

//This holds the last time the MCU sent data over XBee.
unsigned long timeLastSent = 0;

//Add and element at the end of the list
void addRxData(Rx_data rd){
	RxDataList[RxDataListTail].macAddr		= rd.macAddr;
	RxDataList[RxDataListTail].payload		= rd.payload;
	RxDataList[RxDataListTail].payloadSize	= rd.payloadSize;
	RxDataListTail++;
}

//Get the first element that need to be treated
Rx_data getFirstRxData(){
	return RxDataList[RxDataListHead];
}

//To be executed after we have treated the received data entry.
void removeFirstRxData(){
	RxDataList[RxDataListHead].macAddr = 0;
	RxDataList[RxDataListHead].payloadSize = 0;

	free(RxDataList[RxDataListHead].payload);
	RxDataList[RxDataListHead].payload = 0;
	RxDataListHead++;

	rxDataListDefrag();
}

//Test if the list is empty
boolean isRxDataListEmpty(){
	if ((abs(RxDataListTail - RxDataListHead)) > 0){
		return false;
	}
	else{
		return true;
	}
}

//Must be done quite often so we dont run out of space.
void rxDataListDefrag(){
	if (isRxDataListEmpty()){
		RxDataListHead = 0;
		RxDataListTail = 0;
	}
}

//Has to be done as soon as possible after booting.
void initComPort(){
	Serial1.begin(115200);
	xb.setSerial(Serial1);
	RxDataListHead = 0;
	RxDataListTail = 0;
}

/*
Send the payload to the 64-bit address.
The Xbee Series 1 DigiMesh support a maximum of 72 payload bytes even though the DigiMesh protocol specify a payload maximum of 100 bytes.
Putting more bytes in the payload will probably chop it.

We must not go faster than 666Hz (OMFG the number of the beast.)
*/
void sendData(uint64_t address, uint8_t payload[],uint8_t payLoadLen){
	
	//Minimum delay between sending of data: 15ms
	if ((micros() - timeLastSent) > 15000){
		uint32_t addrHigh, addrLow;

		//Masking the high part gets the low part
		addrLow = address & 0x00000000FFFFFFFF;
		//Shifting the high part down 32 bits get the high part
		addrHigh = address >> 32;

		XBeeAddress64 xbAddr = XBeeAddress64(addrHigh, addrLow);

		DMTxRequest txRequest = DMTxRequest(xbAddr, payload, payLoadLen);

		xb.send(txRequest);

		timeLastSent = micros();
	}
}

//Send the payload to broadcast (everyone)
void sendDataBroadcast(uint8_t payload[], uint8_t payLoadLen){

	//This is the standard broadcast MAC address
	uint64_t address = 0x000000000000FFFF;

	//Minimum delay between sending of data: 15ms
	if ((micros() - timeLastSent) > 15000){
		uint32_t addrHigh, addrLow;

		//Masking the high part gets the low part
		addrLow = address & 0x00000000FFFFFFFF;
		//Shifting the high part down 32 bits get the high part
		addrHigh = address >> 32;

		XBeeAddress64 xbAddr = XBeeAddress64(addrHigh, addrLow);

		DMTxRequest txRequest = DMTxRequest(xbAddr, payload, payLoadLen);

		xb.send(txRequest);

		timeLastSent = micros();
	}
}

/*
This gets any packet that is ready to be read.
This function should be executed as often as possible.
*/
void comGetPacket(){
	//Reads all available serial bytes until a packet is parsed, an error occurs, or the buffer is empty.
	xb.readPacket();
	/*
	Beware of this badly chosen function name(.isAvailable).
	This function basically return true if the parsing of the packet was done without errors.
	*/
	if (xb.getResponse().isAvailable()){
		switch (xb.getResponse().getApiId()){
		case DM_RX_RESPONSE:

			//This must be done exactly like that.
			DMRxResponse dmrxr = DMRxResponse();
			xb.getResponse().getDMRxResponse(dmrxr);

			//Getting payload length
			uint8_t dataLen = dmrxr.getDataLength();

			//Getting MAC address as a 64-bit unsigned integer
			uint64_t macAddr = (dmrxr.getRemoteAddress64().getMsb());
			macAddr = macAddr << 32;
			macAddr = macAddr | dmrxr.getRemoteAddress64().getLsb();

			//Getting payload
			uint8_t* payload = (uint8_t*)malloc(dataLen);
			memset(payload, 0, dataLen);
			memcpy(payload, dmrxr.getData(), dataLen);

			//Add informations to the proper structure
			Rx_data newRxData;
			newRxData.macAddr = macAddr;
			newRxData.payload = payload;
			newRxData.payloadSize = dataLen;

			//Add the structure to the list.
			addRxData(newRxData);

			//DEBUG_SP("Received "); DEBUG_SP(dataLen); DEBUG_SP(" bytes from "); printLLNumber(macAddr, HEX); DEBUG_SPL();
			
			break;
		}
	}
}

void printLLNumber(unsigned long long n, uint8_t base)
{
	unsigned char buf[16 * sizeof(long)]; // Assumes 8-bit chars. 
	unsigned long long i = 0;

	if (n == 0) {
		DEBUG_SP('0');
		return;
	}

	while (n > 0) {
		buf[i++] = n % base;
		n /= base;
	}

	for (; i > 0; i--)
		DEBUG_SP((char)(buf[i - 1] < 10 ?
		'0' + buf[i - 1] :
		'A' + buf[i - 1] - 10));
}

//If data is ready to be intepreted, intepret it
void pollCommands(){
	if (isRxDataListEmpty() == false)
		dispatchCommands();
}

//Get a Rx_data and add an appropriate task depending on the payload.
//TODO: Time the different cases
void dispatchCommands(){
	Rx_data data		= getFirstRxData();
	uint64_t macAddr	= data.macAddr;
	uint8_t dataLen		= data.payloadSize;
	uint8_t* payload	= data.payload;

	switch (dataLen){
	case 13:
		//Broadcast position received
		if ((*(data.payload) == 'P') && (*(data.payload + 1) == 'O') && (*(data.payload + 2) == 'S')){
			//DEBUG_SP("Broadcast position received from "); printLLNumber(macAddr, HEX); DEBUG_SPL();
			broadcastPositionCommand();
		}
		break;
	case 3:
		//Demand position received
		if ((*(data.payload) == 'B') && (*(data.payload + 1) == 'R') && (*(data.payload + 2) == 'C')){
			//DEBUG_SP("Demand position received from "); printLLNumber(macAddr, HEX); DEBUG_SPL();
			demandPositionCommand();
		}
		break;
	case 8:
		//Broadcast new target received
		if ((*(data.payload) == 'N') && (*(data.payload + 1) == 'E') && (*(data.payload + 2) == 'W')){
			//DEBUG_SP("Broadcast new target received from "); printLLNumber(macAddr, HEX); DEBUG_SPL();
			newTargetCommand();
		}
		break;
	default:
		//DEBUG_SP("Received unknown "); DEBUG_SP(dataLen); DEBUG_SP(" bytes from "); printLLNumber(macAddr, HEX); DEBUG_SPL();
		removeFirstRxData();
		break;
	}
	
}