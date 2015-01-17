#ifndef DYNSCHED_H
#define DYNSCHED_H

#include "Arduino.h"
#include "Communication.h"

//Define an abstract 'function' type.
typedef void(*function)();

/*
Priorities a task can have.
PRIORITY_TOP tasks can only be added using interrupts.
PRIORITY_HIGH tasks can be added using software.
PRIORITY_NORMAL is the rest.

An interrupt would stop the CPU and treat interrupt request using a TODO handler.
Then the handle might add some HIGH or NORMAL tasks to queues. So TOP tasks have absolute priority.
A HIGH priority task cannot interrupt any ongoing NORMAL tasks but is treated right after the NORMAL task is finished.
The execution order of same priority tasks is FIFO.
*/
typedef enum SCHED_PRIORITIES {
	PRIORITY_TOP,
	PRIORITY_HIGH,
	PRIORITY_NORMAL
};

/*
Define the fields of a task.
taskFunction is the code that will be executed ar run-time.
priority is explained in SCHED_PRIORITIES
isPermanent define weather the task can get removed or not. Only NORMAL priority tasks can be permanent.
nextItem point to the next task in the context of a linked list.
*/
typedef struct task{
	function			taskFunction;
	SCHED_PRIORITIES	priority;
	bool				isPermanent;
	struct task*		nextItem;
} Task;

//Flag used to indicate that a new HIGH priority task has beed added and need to be treated.
//Instanced in DynSched.cpp
extern int HIGH_TASK_FLAG;

//Heads of our linked list, instance in DynSched.cpp
extern Task headTopTask;
extern Task headHighTask;
extern Task headNormalTask;

//Interrupt Service Routines
void lineSensor1(void); //Line Sensor 1
void lineSensor2(void); //Line Sensor 2
void lineSensor3(void); //Line Sensor 3
void lineSensor4(void); //Line Sensor 4

//Permanent default routines
void checkSerial(void);

//Receive communication on UART0 handler
void rxComHandler(void);

//Queues iteration routines.
void iterateHigh(void);
void iterateNormal(void);

//Task execution
Task* execTask(Task* task);

//Task list management
void addTask(Task *toAdd);
void removeTask(Task *toRemove);
Task* getHead(int priority);
Task* getLast(int priority);
Task* getPrevious(Task *task);
Task* getNext(Task* task);
Task* newTask(void);

//Task setters
Task* setPriority(Task* task, int priority);
Task* setPermanent(Task* task, bool permanent);
Task* setFunction(Task* task, function taskFunction);

//Task getters
bool isPermanent(Task* task);
bool isLast(Task* task);
int taskListSize(SCHED_PRIORITIES priority);

#endif //DYNSCHED_H