#ifndef Scheduler_H_
#define Scheduler_H_

#include "TaskControlBlock.h"
#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>

void scheduler(TCB* mData1, TCB* mData2[8]);

void fillSchedule(TCB *taskQueue, TCB *dataPtr);

void shrinkSchedule(TCB *dataPtr);

void insertNode(TCB* node);

void removeNode(TCB* node);

#endif
