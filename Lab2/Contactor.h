#ifdef __cplusplus  
extern "C" { 
#endif 

#ifndef Contactor_H_
#define Contactor_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>

#define TRUE 1
#define FALSE 0

typedef struct contactorTaskData {
	bool* contactState;              // pointer to current state of each alarm
    bool* toggleFlag;                // pointer signaling soc has updated 
} contactorData;


void contactorTask (void* mData);

void updateContactor(bool* contactStatePtr, bool* toggleFlag);
#endif

#ifdef __cplusplus 
} 
#endif 
