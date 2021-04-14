#ifdef __cplusplus  
extern "C" { 
#endif 

#ifndef Alarm_H_
#define Alarm_H_


#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>

#define TRUE 1
#define FALSE 0

#define STATE1 0
#define STATE2 1
#define STATE3 2

#define CURRTIMER 2
#define VOLTTIMER 3
#define NUMSTATES 3

typedef struct alarmTaskData {
	float *hvilAlarm, *currentAlarm, *voltageAlarm;               // pointer to current state of each alarm
    bool *currentAlarmFlag, *voltageAlarmFlag, *hvilAlarmFlag;    // pointer signaling soc has updated 
    int *timeCount;                                               // pointer that helps sync variable rotation
} alarmData;


void alarmTask (void* mData);

void updateHVILAlarm(float* hvilAlarmPtr, bool* hvilAlarmFlag, int* counterPtr);

void updateCurrentAlarm(float* currentAlarmPtr, bool* currentAlarmFlag, int* counterPtr);

void updateVoltageAlarm(float* voltageAlarmPtr, bool* voltageAlarmFlag, int* counterPtr);

#endif

#ifdef __cplusplus 
} 
#endif 
