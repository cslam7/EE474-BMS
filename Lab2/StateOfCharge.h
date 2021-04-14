#ifdef __cplusplus  
extern "C" { 
#endif 

#ifndef StateOfCharge_H_
#define StateOfCharge_H_


#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>

#define TRUE 1
#define FALSE 0
#define NUMSTATES 3

typedef struct socTaskData {
    bool* socFlag;         // pointer signaling soc has updated 
	float* soc;            // pointer to present soc
    int* timeCount;        // pointer to global task sync
} socData;


void socTask(void* mData);

void updateSOC(float* socPtr, bool* socFlag, int* counterPtr);
#endif

#ifdef __cplusplus 
} 
#endif 
