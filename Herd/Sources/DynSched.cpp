#include "DynSched.h"

//For comGetPacket()
#include "Communication.h"

int HIGH_TASK_FLAG = LOW;

Task headHighTask = { NULL, PRIORITY_HIGH, true, NULL };
Task headNormalTask = { NULL, PRIORITY_NORMAL, true, NULL };

/*
Notes to $self.

attachInterrupt(pin, taskHandler, HIGH || LOW || CHANGE || RISING || FALLING)

Possibles interrupts:
	- Communication received over XBee
		- Pin 0
			attachInterrupt(0,rxComHandler,RISING); //Low-state if no transmission
	- Line detected
		- Pin 13
			attachInterrupt(13,lineSensor1,RISING); //To test
		- Pin 14
			attachInterrupt(14,lineSensor2,RISING);
		- Pin 15
			attachInterrupt(15,lineSensor3,RISING);
		- Pin 16
			attachInterrupt(16,lineSensor4,RISING);
Anything else?
*/

//ISR fired when we sense a black line on sensor 1
void lineSensor1(void){
	__disable_irq();
	//TODO Set the appropriate line sensing variables and create a PRIORITY_HIGH task of deducing the position.
	__enable_irq();
}

//ISR fired when we sense a black line on sensor 2
void lineSensor2(void){
	__disable_irq();
	//TODO Set the appropriate line sensing variables and create a PRIORITY_HIGH task of deducing the position.
	__enable_irq();
}

//ISR fired when we sense a black line on sensor 3
void lineSensor3(void){
	__disable_irq();
	//TODO Set the appropriate line sensing variables and create a PRIORITY_HIGH task of deducing the position.
	__enable_irq();
}

//ISR fired when we sense a black line on sensor 4
void lineSensor4(void){
	__disable_irq();
	//TODO Set the appropriate line sensing variables and create a PRIORITY_HIGH task of deducing the position.
	__enable_irq();
}

//fired when we receive a serial communication
void rxComHandler(void){
	__disable_irq();
	//Get the Xbee packet data onto the list.
	comGetPacket();
	__enable_irq();
}

//Iterate over PRIORITY_HIGH tasks and execute them.
void iterateHigh(void){
	Task* task = getHead(PRIORITY_HIGH)->nextItem;
	
	int tls = taskListSize(PRIORITY_HIGH);
	while (tls--){
		task = execTask(task);
	}
	//All HIGH priority tasks have been treated, lower the flag.
	HIGH_TASK_FLAG = LOW;
}

//Iterate over PRIORITY_NORMAL tasks and execute them.
void iterateNormal(void){
	//The first task is the one next to the head.
	Task* task = getHead(PRIORITY_NORMAL)->nextItem;
	int tls = taskListSize(PRIORITY_NORMAL);
	while (tls--){
		if (HIGH_TASK_FLAG)
			iterateHigh();
		task = execTask(task);
	}
}


//Execute a task and potentially remove it from the list.
//Return a pointer to the nextItem
Task* execTask(Task* task){
	//Next task
	Task* toRet = task->nextItem;
	//To protect from a common mistake.
	if (task != NULL && task->taskFunction != NULL){
		//Execute this tasks function
		task->taskFunction();
		//If the task is not a permanent one.
		if (!isPermanent(task)){
			//Remove is from the queue.
			removeTask(task);
			//Free the space
			free(task);
		}
	}
	return toRet;
}

//Adds a taks to the end of the list
void addTask(Task *toAdd){
	Task *last = getLast(toAdd->priority);

	last->nextItem = toAdd;
	toAdd->nextItem = NULL;

	//If we are adding a HIGH priority task, set the flag.
	if (toAdd->priority == PRIORITY_HIGH)
		HIGH_TASK_FLAG = HIGH;
}

//Get the appropriate Task list head
Task* getHead(int priority){
	if (priority == PRIORITY_HIGH)
		return &headHighTask;
	if (priority == PRIORITY_NORMAL)
		return &headNormalTask;
}

//Get the last item in the list
Task* getLast(int priority){
	Task *current = getHead(priority);

	while (current->nextItem != NULL) {
		current = current->nextItem;
	}

	return current;
}

//Get the item right before the submitted item.
Task* getPrevious(Task *task){
	Task *current, *head = getHead(task->priority);

	current = head;
	while (current->nextItem != task){
		current = current->nextItem;
	}
	return current;
}

//Get the item right after the submitted item.
Task* getNext(Task* task){
	return task->nextItem;
}

//Remove the specified task from the appropriate list.
//THIS DOES NOT FREE THE TASK!
void removeTask(Task *toRemove){
	Task *previous	= getPrevious(toRemove);
	Task *next		= getNext(toRemove);
	previous->nextItem = next;
}

//Creates a new initialized task
Task* newTask(void){
	Task* task = (Task*)malloc(sizeof(Task));
	
	task->isPermanent	= false;
	task->nextItem		= NULL;
	task->priority		= PRIORITY_NORMAL;
	task->taskFunction	= NULL;

	return task;
}

//Set the priority to an already created Task
Task* setPriority(Task* task, int priority){
	task->priority = (SCHED_PRIORITIES)priority;
	return task;
}

//Set the permanence to an already created Task
//Only NORMAL priorities tasks can be permanent
Task* setPermanent(Task* task, bool permanent){
	if (task->priority == PRIORITY_NORMAL)
		task->isPermanent = permanent;
	return task;
}

//Set the function to an already created Task
Task* setFunction(Task* task, function taskFunction){
	task->taskFunction = taskFunction;
	return task;
}

//Return the permanence of the task.
bool isPermanent(Task* task){
	return task->isPermanent;
}

//Return true if the task is the last in his 
bool isLast(Task* task){
	if (task->nextItem != NULL) return false;
	return true;
}

//Return the number of items in the list
int taskListSize(SCHED_PRIORITIES priority){
	if (priority != PRIORITY_HIGH && priority != PRIORITY_NORMAL) return -1;

	Task* task = &headNormalTask;
	if (priority == PRIORITY_HIGH)
		task = &headHighTask;
	
	int count = 0;
	while (task->nextItem != NULL){
		task = task->nextItem;
		count++;
	}
	return count;
}