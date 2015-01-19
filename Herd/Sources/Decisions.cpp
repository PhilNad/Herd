#include "Decisions.h"


//Will take care of all decision-making processes most notably when we will need to move through the board.

//No operation
void doNothing(){ return; }

//Initialize the functions associated with sensors patterns
void initAssociatedFunctions(){
	for (int i = 0; i <= 16; ++i){
		setAssociatedFunction(i, doNothing);
	}
}

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

/*
How to choose on which case to move?

1.	Eliminate all non-free cases from choices.
2.	From all choices that are left, choose the one that would get us closer to our target.
		If no choice would get us closer to our target, go to (3)
3.	Calculate the sum of other bots directions. Choose the choice that would be the more
		toward that resulting direction.
4.	Move to the chosen choice.
*/

Direction bot_to_target(Bot b){
	Point position	= b.position;
	Point target	= b.target;

	Direction d;
	d.x = target.x - position.x;
	d.y = target.y - position.y;
	return d;
}

Direction sum_bots_directions(){
	short number_of_bots = getNumberOfBots();

	Direction sum;

	for (short i = 0; i < number_of_bots;++i){
		Bot b = getNthBot(i);
		sum.x += bot_to_target(b).x;
		sum.y += bot_to_target(b).y;
	}

	return sum;
}

Direction get_opposite(Direction d){
	Direction r;
	r.x = 0 - d.x;
	r.y = 0 - d.y;
	return r;
}

//Choose the best point we should move to
Point nextMove(){
	Bot self = getMyself();
	
	Point result;
	result.x = self.position.x;
	result.y = self.position.y;

	//This is all the possible different moves
	Point choices[5] = {
		{	self.position.x		,	self.position.y		},
		{	self.position.x - 1	,	self.position.y		},
		{	self.position.x + 1 ,	self.position.y		},
		{	self.position.x		,	self.position.y - 1 },
		{	self.position.x		,	self.position.y + 1 },
	};

	//1)
	for (Point c : choices){
		if (!isCaseFree(c.x, c.y))
			c.x = -1;
			c.y = -1;
	}

	//2)
	Direction selfDir = bot_to_target(self);
	//Initialized to our current position
	auto shortestDistance = abs(selfDir.x) + abs(selfDir.y);

	for (Point c : choices){
		if (c.x != -1 && c.y != -1){
			auto dist = abs(self.target.x - c.x) + abs(self.target.y - c.y);
			if (dist < shortestDistance)
				shortestDistance	= dist;
				result.x = c.x;
				result.y = c.y;

		}
	}

	//3)
	if (result.x == self.position.x && result.y == self.position.y){
		Direction s = sum_bots_directions();
		if (abs(s.x) < abs(s.y)){
			//Move in s.x direction
			result.y = self.position.y;
			if (s.x >= 0)
				result.x = self.position.x + 1;
			else
				result.x = self.position.x - 1;
		}
		else{
			//Move in s.y direction
			result.x = self.position.x;
			if (s.y >= 0)
				result.y = self.position.y + 1;
			else
				result.y = self.position.y - 1;
		}
	}
	return result;
}