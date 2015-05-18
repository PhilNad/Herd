#include "Decisions.h"


//Will take care of all decision-making processes most notably when we will need to move through the board.



//No operation
void doNothing(){}

//Initialize the functions associated with sensors patterns
void initAssociatedFunctions(){
	for (int i = 0; i <= 16; ++i){
		setAssociatedFunction(i, doNothing);
	}
	initStabilization();
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

//Sets associated functions for right and left IR sensors
//These sensors are used for stabilization
//If the sides sensors are not in the same state, the robot needs to be stabilized
void initStabilization(){
	setAssociatedFunction(2, stabilize);
	setAssociatedFunction(3, stabilize);
	setAssociatedFunction(4, stabilize);
	setAssociatedFunction(5, stabilize);
	setAssociatedFunction(10, stabilize);
	setAssociatedFunction(11, stabilize);
	setAssociatedFunction(12, stabilize);
	setAssociatedFunction(13, stabilize);
}

//Executed when lateral sensors are not in the same state
void stabilize(){
	unsigned char pattern = calculateSensorPattern();
	stopCondition sc;
	MotorAction ma;
	//Leaning to the left
	if (is_set(pattern, RIGHT_SENSOR)){
		//This means: "Go forward right until the left sensor is transitioning from white to black"
		ma = FORWARD_RIGHT;
		sc.addSensorTransition(LEFT_SENSOR, sc.whiteToBlack);
	}
	else{
		//Leaning to the right
		if (is_set(pattern, LEFT_SENSOR)){
			//This means: "Go forward left until the right sensor is transitioning from white to black"
			ma = FORWARD_LEFT;
			sc.addSensorTransition(RIGHT_SENSOR, sc.whiteToBlack);
		}
	}
	sc.setStop();
	//Make the movement
	executeMotorAction(ma, 33);
}

//Decide what we should do depending on the context
void actionDecision(){
	//Are we associated with a target?
	Bot myself = getMyself();
	if (myself.target.x > 0 && myself.target.y > 0){
		//If we are not moving, its time to generate new move
		if (!isMoving()){
			//Get our next checkpoint
			Point cp = nextMove();
			//Find corresponding motor action
			MotorAction ma = correspondingAction(cp);
			//Generate appropriate stop condition
			stopCondition sc = generateForMotorAction(ma);
			//Associate pattern with stop action
			sc.setStop();
			//Start movement at 33% of full speed
			executeMotorAction(ma, 33);
		}
	}
	else{
		//Is there any free target?
		if (hasFreeTarget()){
			//Associate with a target and announce it.
			Point target	= getTarget();
			myself.target	= target;
			demandPositionCommand();
		}
	}
}

Direction bot_to_target(Bot b){
	Point position = b.position;
	Point target = b.target;

	Direction d;
	d.x = target.x - position.x;
	d.y = target.y - position.y;
	return d;
}

//Calculate the vector that represent the summ of all other bots directions
Direction sum_bots_directions(){
	short number_of_bots = getNumberOfBots();

	Direction sum;

	for (short i = 0; i < number_of_bots; ++i){
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

//Add two points together
//TODO: put that in the Point struct as an operator
Point addPoints(Point a, Point b){
	Point p;
	p.x = a.x + b.x;
	p.y = a.y + b.y;
	return p;
}

//Calculate the modulus of a vector
double modulus(Direction d){
	return sqrt(d.x*d.x + d.y*d.y);
}

//Calculate the similarity between two vectors
//Least similar: -1 ; More similar:1
double cos_similarity(Direction d, Direction e){
	double dotProduct = d.x*e.x + d.y*e.y;
	double mod_multiplied = modulus(d)*modulus(e);
	if (mod_multiplied == 0)
		mod_multiplied = 0.0001;
	return dotProduct / mod_multiplied;
}

//Return the point around current position that we would need to move on to follow d direction.
Point direction_optimized_point(Direction d){
	//Current bot
	Bot self = getMyself();
	//This is all the possible different moves
	Point choices[5] = {
		{ self.position.x, self.position.y },	  //No move
		{ self.position.x - 1, self.position.y }, //Move left
		{ self.position.x + 1, self.position.y }, //Move right
		{ self.position.x, self.position.y - 1 }, //Move down
		{ self.position.x, self.position.y + 1 }, //Move up
	};

	//Best choice as an index of choices
	int best_choice = 0;
	double best_choice_cos_sim = -2.0;

	//Cycle through free cases and find the optimal one
	for (int i = 0; i < 5; ++i){
		if (isCaseFree(choices[i].x, choices[i].y)){
			Direction e;
			e.x = choices[i].x;
			e.y = choices[i].y;

			double similarity = cos_similarity(d, e);
			if (similarity > best_choice_cos_sim){
				best_choice			= i;
				best_choice_cos_sim = similarity;
			}
		}
	}
	return choices[best_choice];
}

//1. Remove occupied cases from list
//2. Initialize default result to move toward general bots direction
//3. Try to find one that will get us nearer our target.
Point nextMove(){
	//Current bot
	Bot self = getMyself();

	//Our current direction
	Direction our_direction = bot_to_target(self);

	//General bots direction
	Direction sum_directions = sum_bots_directions();

	//Initialized to optimized case for general direction
	Point result = direction_optimized_point(sum_directions);

	//Try to find a move that would get us closer to our target
	Point best_move = direction_optimized_point(our_direction);
	if (best_move.x != self.position.x && best_move.y != self.position.y)
		result = best_move;

	return result;
}

//Find which action would correspond to moving to the submitted Point
MotorAction correspondingAction(Point checkpoint){
	//Current bot
	Bot self = getMyself();
	if (checkpoint.x == (self.position.x - 1) && checkpoint.y == (self.position.y - 0))//Move left
		return MotorAction::FORWARD_LEFT;
	if (checkpoint.x == (self.position.x + 1) && checkpoint.y == (self.position.y - 0))//Move right
		return MotorAction::FORWARD_RIGHT;
	if (checkpoint.x == (self.position.x - 0) && checkpoint.y == (self.position.y - 1))//Move down
		return MotorAction::BACKWARD;
	if (checkpoint.x == (self.position.x - 0) && checkpoint.y == (self.position.y + 1))//Move up
		return MotorAction::FORWARD;
	return MotorAction::STOP;
}

//Generate a stop condition based on the movement that will be done.
stopCondition generateForMotorAction(MotorAction ma){
	stopCondition sc;
	if (ma == BACKWARD || ma == BACKWARD_RIGHT || ma == BACKWARD_LEFT)
		sc.addSensorTransition(BACK_SENSOR, sc.whiteToBlack);
	if (ma == FORWARD || ma == FORWARD_RIGHT || ma == FORWARD_LEFT)
		sc.addSensorTransition(FRONT_SENSOR, sc.whiteToBlack);
	return sc;
}

//Start a motor action at percentage speed
void executeMotorAction(MotorAction ma, int speed){
	switch (ma){
	case MotorAction::FORWARD:
		goForward(speed);
	case MotorAction::FORWARD_RIGHT:
		goForwardRight(speed);
	case MotorAction::FORWARD_LEFT:
		goForwardLeft(speed);
	case MotorAction::BACKWARD:
		goBackward(speed);
	case MotorAction::BACKWARD_RIGHT:
		goBackwardRight(speed);
	case MotorAction::BACKWARD_LEFT:
		goBackwardLeft(speed);
	}
}