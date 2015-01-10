#include "Board.h"
#include "Communication.h"

//TODO: Dynamic memory allocation

//List of bot, the index will later be used to identify the bot
//Size: 16 bytes * 25 = 400 bytes
Bot bots[X_SIZE*Y_SIZE];
//This keeps track of the number of bots in our list and helps keep things efficient
unsigned short number_of_bots = 0;

//Special information holder for this bot.
Bot myself;

//List of not-yet-associated targets
//Size: 4 bytes * 25 = 100 bytes
Point freeTargets[X_SIZE*Y_SIZE];
//This keeps track of the number of free targets in our list and helps keep things efficient
unsigned short number_of_free_targets = 0;

//Matrix of space
//Size: 4 byte * 25 = 100 bytes
Case Board[X_SIZE][Y_SIZE];

//Initialize the board, called on startup
void initBoard(){
	//Initlize myself
	myself.position.x	= STARTING_X;
	myself.position.y	= STARTING_Y;
	myself.macAddr		= 0;
	myself.target.x		= 0;
	myself.target.y		= 0;

	//Initlialize bots list
	for (auto b : bots){
		b.macAddr		= 0;
		b.position.x	= 0;
		b.position.y	= 0;
		b.target.x		= 0;
		b.target.y		= 0;
	}

	//Initialize free targets list
	for (auto t : freeTargets){
		t.x = 0;
		t.y = 0;
	}
		
	//Initlialize board
	for (auto c : Board)
		setCaseFree(*c);
}

//Add a recently discovered target to the table of free targets
unsigned short addTarget(Point position){
	freeTargets[number_of_free_targets] = position;
	++number_of_free_targets;
	return number_of_free_targets;
}

//Get the first target in our FIFO list and remove it from the list
Point getTarget(){
	Point target = freeTargets[0];
	for (int i = 1; i < number_of_free_targets; i++){
		freeTargets[i - 1] = freeTargets[i];
	}
	return target;
}

//Search the free targets list for a particular point, if found, returns its index, if not return -1;
short searchTarget(Point position){
	for (short i = 0; i < number_of_free_targets; i++){
		if (freeTargets[i].x == position.x && freeTargets[i].y == position.y)
			return i;
	}
	return -1;
}

//Remove a target from the list using its index
void removeTarget(uint16_t index){
	for (short i = index; i < number_of_free_targets; i++){
		freeTargets[i] = freeTargets[i + 1];
	}
}

//Add a recently discovered bot to the table of bots and return its index
unsigned short addBot(Bot bot){
	bots[number_of_bots] = bot;
	++number_of_bots;
	return number_of_bots;
}

//Remove a bot from the list using its index
void removeBot(uint16_t index){
	for (short i = index; i < number_of_bots; i++){
		bots[i] = bots[i + 1];
	}
}

//Get a bot associated with a MAC address. Return its index in the list. If not found, returns -1
short getBotFromMAC(uint64_t macAddr){
	for (short i = 0; i < number_of_bots; i++){
		if (bots[i].macAddr == macAddr){
			return i;
		}
	}
	return -1;
}

//Get a bot associated with a position Returns its index in the list, If not found, returns -1.
short getBotFromPosition(Point position){
	for (short i = 0; i < number_of_bots; i++){
		if (bots[i].position.x == position.x && bots[i].position.y == position.y)
			return i;
	}
	return -1;
}

//Return the target associated with a particular bot
Point getBotTarget(short index){
	return bots[index].target;
}

//Return the position associated with a particular bot
Point getBotPosition(short index){
	return bots[index].position;
}

//Return the case associated with a particular bot
Case getBotCase(short index){
	return Board[getBotPosition(index).x][getBotPosition(index).y];
}

//Set a particular bot's position
void setBotPosition(short index, Point position){
	bots[index].position = position;
}

//Set a particular bot's target's position
void setBotTarget(short index, Point position){
	bots[index].target = position;
}

//Getter for myselef
Bot getMyself(){
	return myself;
}

//Get a case by his position
Case getCase(uint16_t x, uint16_t y){
	return Board[x][y];
}
//Get a case by his position
Case getCase(Point point){
	return Board[point.x][point.y];
}

//Return true if the case is free of any obect
bool isCaseFree(uint16_t x, uint16_t y){
	return Board[x][y].isFree;
}
//Return true if the case is free of any obect
bool isCaseFree(Case __case){
	return __case.isFree;
}
//Return true if the case is free of any obect
bool isCaseFree(Point point){
	return Board[point.x][point.y].isFree;
}

//Return true if a bot is present on the case
bool isCaseBotPresent(uint16_t x, uint16_t y){
	return Board[x][y].isBotPresent;
}
//Return true if a bot is present on the case
bool isCaseBotPresent(Case __case){
	return __case.isBotPresent;
}
//Return true if a bot is present on the case
bool isCaseBotPresent(Point point){
	return Board[point.x][point.y].isBotPresent;
}

//Return the Bot associated with a case
Bot getBotFromCase(uint16_t x, uint16_t y){
	Point p = { p.x = x, p.y = y };
	short index = getBotFromPosition(p);
	return bots[index];
}
//Return the Bot associated with a case
Bot getBotFromCase(Point point){
	short index = getBotFromPosition(point);
	return bots[index];
}
//Return the Bot associated with a case
Bot getBotFromCase(Case __case){
	short index = __case.indexOfPresentBot;
	return bots[index];
}

//Set a case as being free
void setCaseFree(Case __case){
	__case.isFree				= true;
	__case.indexOfPresentBot	= -1;
	__case.isBotPresent			= false;
}

//Set a case as being occupied by a bot
void setCaseBotPresent(Case __case, short index){
	__case.isFree				= false;
	__case.isBotPresent			= true;
	__case.indexOfPresentBot	= index;
}

//Set a case as being occupied by somethign else
void setCaseOccupied(Case __case){
	__case.isFree				= false;
	__case.isBotPresent			= false;
	__case.indexOfPresentBot	= -1;
}