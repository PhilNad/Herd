#ifndef BOARD_H
#define BOARD_H

#include "Arduino.h"
#include "Communication.h"

//Default starting position for a bot
#define STARTING_X 0
#define STARTING_Y 0

//Size of the space
#define X_SIZE 5
#define Y_SIZE 5

//Make sure we have enough memory to hold all information
#define CODE_SIZE	21240
#define TOTAL_FLASH 262144 
#define NEEDED_BOARD_MEMORY_SIZE ((16+4+4)*X_SIZE*Y_SIZE)

//Definition of a point
//Size: 4 bytes
typedef struct point{
	uint16_t x;
	uint16_t y;
} Point;

//Structure defining a bot containing information about position and target
//Size: 16 bytes
typedef struct bot{
	uint64_t	macAddr;
	Point		position;
	Point		target;
} Bot;

//Structure defining a case that will be part of the board
//Size: 4 bytes
typedef struct _case{
	bool		isFree;					//TRUE if the case if free of any obstacle (bot,target,permanentObstacle)
	bool		isBotPresent;			//TRUE if a bot is on the case
	uint16_t	indexOfPresentBot;		//Index of the Bot in the bots array
} Case;

//Initialize the board
void initBoard();

//Related to other targets
unsigned short addTarget(Point position);
Point getTarget();
short searchTarget(Point position);
void removeTarget(uint16_t index);
bool hasFreeTarget();

//Related to other bots
Bot getNthBot(short n);
short getNumberOfBots();
unsigned short addBot(Bot bot);
void removeBot(uint16_t index);
short getBotFromMAC(uint64_t macAddr);
short getBotFromPosition(Point position);
Point getBotTarget(short index);
Point getBotPosition(short index);
Case getBotCase(short index);
void setBotPosition(short index, Point position);
void setBotTarget(short index, Point position);

//Related to myself
Bot getMyself();

//Related to the board
Case getCase(uint16_t x, uint16_t y);
Case getCase(Point point);

bool isCaseFree(uint16_t x, uint16_t y);
bool isCaseFree(Case __case);
bool isCaseFree(Point point);

bool isCaseBotPresent(uint16_t x, uint16_t y);
bool isCaseBotPresent(Case __case);
bool isCaseBotPresent(Point point);

Bot getBotFromCase(uint16_t x, uint16_t y);
Bot getBotFromCase(Point point);
Bot getBotFromCase(Case __case);

void setCaseFree(Case __case);
void setCaseBotPresent(Case __case, short index);
void setCaseOccupied(Case __case);


#endif //BOARD_H