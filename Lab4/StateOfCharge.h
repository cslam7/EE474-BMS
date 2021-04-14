#ifndef StateOfCharge_H_
#define StateOfCharge_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <Arduino.h>
#define FALSE 0
#define TRUE 1
#define NUM_TEMP 4
#define NUM_VOLT 5

#define BOUND_SIZE 2
#define LOWER 0
#define UPPER 1

#define RBATT 0.5
typedef struct socTaskData {
    float* soc;            // pointer to present soc                              
    bool* socFlag;         // pointer signaling soc has updated 
    float* temperature;    // present value of temp
    bool* tempFlag;        // update temp on display
    float* hvCurrent;      // present value of current
    bool* currentFlag;     // update temp on display
    float* hvVoltage;      // present value of voltage
    bool* voltageFlag;     // update temp on display
    // float (*tempArray)[NUM_TEMP];      // possible temperature values in lookup table
    // float (*voltArray)[NUM_VOLT];      // possible voltage values in lookup table
    // float (*dataArray)[NUM_TEMP][NUM_VOLT];      // state of charge percentage data

} socData;

//    soc                 = (&soc, &socFlag, &temperature,
//                           &tempFlag, &hvCurrent, &currentFlag, &hvVoltage,
//                           &voltageFlag, &tempArray, &voltArray, &dataArray);
void socTask(void* mData);

void updateSOC(float *socPtr, bool *socFlagPtr, float *temperaturePtr,
               bool *tempFlagPtr, float *hvCurrentPtr, bool *currentFlagPtr, float *hvVoltagePtr,
               bool *voltageFlagPtr);

void findBoundaries(float *presentValue, float boundArray[], int *lowerBoundIndex,
                    int *upperBoundIndex, int size);

float interpolate_segment(float x0, float y0, float x1, float y1, float x);

void calculateSOC(float *socPtr, bool *socFlagPtr, float *temperaturePtr, float *hvCurrentPtr, 
               float *hvVoltagePtr);
            //    , float *tempArrayPtr, float *voltArrayPtr, float (*dataArrayPtr)[NUM_TEMP][NUM_VOLT]

#endif
