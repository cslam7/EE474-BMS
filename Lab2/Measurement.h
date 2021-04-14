#ifdef __cplusplus  
extern "C" { 
#endif 

#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>


#define TRUE 1
#define FALSE 0

#define NUMSTATES 3

#define STATE1 0
#define STATE2 1
#define STATE3 2

#define OFF 0

#define VOLTTIMER 3
#define CURRTIMER 2

#define HVIL_OPEN   FALSE
#define HVIL_CLOSED TRUE

typedef struct measurementTaskData {

    bool* hvilStatus;               // status of hvil from test circuit
    const byte* hvilPin;            // pin number for reading input for hvil
    float* temperature;             // present value of temp
    float* hvCurrent;               // present value of current
    float* hvVoltage;               // present value of voltage
    bool *hvilFlag, *tempFlag, *currentFlag, *voltageFlag;      // flags for updating fields on display
    int* timeCount;                 // global time counter
} measurementData;


void measurementTask (void* mData);

void updateHVIL(bool* hvilReading, const byte* pin, bool* hvilFlagPtr);

void updateTemperature(float* temperatureReading, bool* tempFlag, int* counterPtr);

void updateHvCurrent(float* currentReading, bool* currentFlag, int* counterPtr);

#endif

#ifdef __cplusplus 
} 
#endif 
