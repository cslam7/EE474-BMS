#ifdef __cplusplus  
extern "C" { 
#endif 

#ifndef StateOfCharge_H_
#define StateOfCharge_H_


#include <stdlib.h>
#include <stdbool.h>
//#include <Arduino.h>
#define FALSE 0

typedef struct socTaskData {
   	bool* socFlag;              // whether soc value has updated
} socData;


void socTask(void* mData);

void updateSOC(bool* socFlag);
#endif

#ifdef __cplusplus 
} 
#endif 
