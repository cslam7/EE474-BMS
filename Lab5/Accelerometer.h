#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include <math.h>
#define TRUE 1
#define FALSE 0

#define BUFFER_SIZE 5
#define BUFFER_FULL 4

#define CONVERT_CM 980
typedef struct accelerometerTaskData {

    float *deltaX;                   // relative position from origion via x axis
    float *deltaY;                   // relative position from origion via y axis
    float *deltaZ;                   // relative position from origion via z axis
    float *distanceTraveled;         // total distance traveled from startup
    float *degreeX;                  // relative rotation from origion via x axis
    float *degreeY;                  // relative rotation from origion via y axis
    float *degreeZ;                  // relative rotation from origion via z axis

    float *oldDeltaX;                // old relative position from origion via x axis
    float *oldDeltaY;                // old relative position from origion via y axis
    float *oldDeltaZ;                // old relative position from origion via z axis
    float *oldDistanceTraveled;      // old distance trveled
    float *oldDegreeX;               // old relative rotation from origion via x axis
    float *oldDegreeY;               // old relative rotation from origion via y axis
    float *oldDegreeZ;               // old relative rotation from origion via z axis

    const byte *pinX;                // analog input pin for acceleration in x-axis
    const byte *pinY;                // analog input pin for acceleration in y-axis
    const byte *pinZ;                // analog input pin for acceleration in z-axis

    bool *deltaXFlag;                // update deltaX on display
    bool *deltaYFlag;                // update deltaY on display
    bool *deltaZFlag;                // update deltaZ on display
    bool *distanceTraveledFlag;      // update distance traveled on display
    bool *degreeXFlag;               // update degreeX on display
    bool *degreeYFlag;               // update degreeY on display
    bool *degreeZFlag;               // update degreeX on display

    int *bufferIndex;                // position of current value in average buffer
    int *bufferFilled;               // if buffer to calculate average is full

    float *bufferX;                  // buffer for x-axis acceleration average
    float *bufferY;                  // buffer for y-axis acceleration average
    float *bufferZ;                  // buffer for z-axis acceleration average

    float *inputX;                   // current x-axis acceleration
    float *inputY;                   // current y-axis acceleration
    float *inputZ;                   // current z-axis acceleration

    float *oldInputX;                // old x-axis acceleration
    float *oldInputY;                // old y-axis acceleration
    float *oldInputZ;                // old z-axis acceleration

    float *velocityX;                // current x-axis velocity
    float *velocityY;                // current y-axis velocity
    float *velocityZ;                // current z-axis velocity

    float *oldVelocityX;             // old x-axis velocity
    float *oldVelocityY;             // old y-axis velocity
    float *oldVelocityZ;             // old z-axis velocity

    float *timeBase;                 // global time base
} accelerometerData;

void accelerometerTask(void* mData);

void integration(float *updateValue, float oldValue, float newValue, float width);

float rollingAverage(float input, float *bufferPtr, int currentIndex, int maxIndex);

void accelInput(float *inputXPtr, float *inputYPtr, float *inputZPtr, 
                float *oldInputXPtr, float *oldInputYPtr, float *oldInputZPtr, 
                int *bufferIndex, float *bufferX, float *bufferY, float *bufferZ,
                int *bufferFilled, const byte *pinX, const byte *pinY, const byte *pinZ,
                bool *deltaXFlagPtr, bool *deltaYFlagPtr, bool *deltaZFlagPtr);
                
void updateRelative(float *deltaXPtr, float *deltaYPtr, float *deltaZPtr,
                    float *oldDeltaXPtr, float *oldDeltaYPtr, float *oldDeltaZPtr,
                    float *inputXPtr, float *inputYPtr, float *inputZPtr,
                    float *oldInputXPtr, float *oldInputYPtr, float *oldInputZPtr,
                    float *velocityXPtr, float *velocityYPtr, float *velocityZPtr,
                    float *oldVelocityXPtr, float *oldVelocityYPtr, float *oldVelocityZPtr,
                    const byte *pinX, const byte *pinY, const byte *pinZ,
                    bool *deltaXFlagPtr, bool *deltaYFlagPtr, bool *deltaZFlagPtr,
                    float *timeBasePtr, float *bufferX, float *bufferY, float *bufferZ,
                    int *bufferFilled);

void updateTotal(float *distanceTraveledPtr, float *deltaXPtr, float *deltaYPtr,
                 float *oldDeltaXPtr, float *oldDeltaYPtr, float *oldDistanceTraveledPtr,
                 bool *distanceTraveledFlagPtr, bool *deltaXFlagPtr, bool *deltaYFlagPtr);

void updateDegree(float *degreeXPtr, float *degreeYPtr, float *degreeZPtr,
                  float *oldDegreeXPtr, float *oldDegreeYPtr, float *oldDegreeZPtr,  
                  float *inputXPtr, float *inputYPtr, float *inputZPtr,
                  bool *degreeXFlagPtr, bool *degreeYFlagPtr, bool *degreeZFlagPtr);

#endif
