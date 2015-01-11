#include "Commands.h"

//TODO: How do we detect a bot is removed from the board? We can detect a new bot because he sends his position but what about
//		a bot that is just shutdown? Probably nothing to do....Then what about a bot that sends a message to tell everyone he is
//		going to shutdown and THEN shutdown? To implement. Also, we need to our architecture the fact that a target has an origin
//		AND a destination. For now we juste take into account its origin. Generate a random destination for each picked up target?

//POS command
//Parse the payload and update the robots position table
void broadcastPositionCommand(Rx_data data){
	uint64_t macAddr	= data.macAddr;
	uint8_t dataLen		= data.payloadSize;
	uint8_t* payload	= data.payload;
	
	//0123456789012
	//POS XXYY XXYY
	char bot_x_position_array[3]	= { *(payload + 4),		*(payload + 5),		0 };
	char bot_y_position_array[3]	= { *(payload + 6),		*(payload + 7),		0 };
	char target_x_position_array[3] = { *(payload + 9),		*(payload + 10),	0 };
	char target_y_position_array[3] = { *(payload + 11),	*(payload + 12),	0 };

	int bot_x_position		= atoi(bot_x_position_array);
	int bot_y_position		= atoi(bot_y_position_array);
	int target_x_position	= atoi(target_x_position_array);
	int target_y_position	= atoi(target_y_position_array);

	Point bot_position, target_position;
	bot_position.x		= bot_x_position;
	bot_position.y		= bot_y_position;
	target_position.x	= target_x_position;
	target_position.y	= target_y_position;

	//If the target is in the free target list, remove it from there because a bot have associated with it
	short free_target_index = searchTarget(target_position);
	if (free_target_index >= 0){
		removeTarget(free_target_index);
	}

	//Update remote bot information
	short bot_index = getBotFromMAC(macAddr);

	//If the bot was not already listed, we need to add it.
	//If the bot was already listed, we need to take care of old information
	if (bot_index == -1){
		Bot new_bot;
		new_bot.macAddr		= macAddr;
		new_bot.position	= bot_position;
		new_bot.target		= target_position;
		bot_index			= addBot(new_bot);
		DEBUG_SP("NEW BOT with mac: "); printLLNumber(macAddr, HEX); DEBUG_SP(" at position "); DEBUG_SP(bot_position.x); DEBUG_SP(":"); DEBUG_SPL(bot_position.y);
	}else{
		//Make the case that was occupied by the bot, free
		Case c = getBotCase(bot_index);
		setCaseFree(c);
		DEBUG_SP("New position for bot="); DEBUG_SP(bot_index); DEBUG_SP(" at "); DEBUG_SP(bot_position.x); DEBUG_SP(":"); DEBUG_SPL(bot_position.y);
	}
	
	if (bot_index != -1){
		//Make the new case occupied by the bot, occupied!
		Case c = getBotCase(bot_index);
		setCaseBotPresent(c, bot_index);

		//Introduce our new informations in our bots list
		setBotPosition(bot_index, bot_position);
		setBotTarget(bot_index, target_position);
	}
	else{
		DEBUG_SPL("ERROR: Created bot cannot be found.");
	}
}

//BRC command
//Send our position and target
//TODO: Change sprintf for something more secure
void demandPositionCommand(){
	Bot myself = getMyself();

	Point	myPosition	= myself.position;
	Point	myTarget	= myself.target;

	uint8_t payload[13]{};

	int result = sprintf((char*)payload, "POS %02d%02d %02d%02d", myself.position.x, myself.position.y, myself.target.x, myself.target.y);
	
	if (result != 13){
		DEBUG_SP("Failed to send correct POS command. Returned : "); DEBUG_SPL(result);
	}
	else{
		DEBUG_SP("Sending our position: "); DEBUG_SPL((char*)payload);
		sendDataBroadcast(payload, 13);
	}
}

//NEW command
//Update table of targets
void newTargetCommand(Rx_data data){
	uint8_t* payload	= data.payload;
	char target_x_position_array[3] = { *(payload + 4), *(payload + 5), 0 };
	char target_y_position_array[3] = { *(payload + 6), *(payload + 7), 0 };

	int target_x_position = atoi(target_x_position_array);
	int target_y_position = atoi(target_y_position_array);

	Point target_position;
	target_position.x = target_x_position;
	target_position.y = target_y_position;

	//If the target is not already in the free target list, add it
	short free_target_index = searchTarget(target_position);
	if (free_target_index < 0){
		addTarget(target_position);
	}
	DEBUG_SP("Discovered target at "); DEBUG_SP(target_position.x); DEBUG_SP(":"); DEBUG_SPL(target_position.y);
}