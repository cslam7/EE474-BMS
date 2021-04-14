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

#define HVIL_OPEN FALSE
#define HVIL_CLOSED TRUE
typedef struct alarmTaskData {
	volatile float *hvilAlarm;                        // pointer to hvil alarm
	float *currentAlarm, *voltageAlarm;               // pointer to current state of current and voltage alarm
    bool *currentAlarmFlag, *voltageAlarmFlag;        // pointer to update flags for current and voltage
    volatile bool *hvilAlarmFlag;                     // pointer signaling hvil alarm is updated
    bool *HVIL;                                       // pointer to hvil status 
    float *hvCurrent, *hvVoltage;                     // pointer to current/voltage values
    volatile bool *acknowledgeFlag;                   // if there is an active but unacknowledged alarm
    volatile bool *acknowledgeButtonExistsFlag;       // whether there an acknowledge button exists
} alarmData;

void alarmTask (void* mData);

void updateHVILAlarm(float* hvilAlarmPtr, bool* hvilAlarmFlag, bool *hvilReading);

void updateCurrentAlarm(float* currentAlarmPtr, bool* currentAlarmFlag, float *currentReading, bool *acknowledgeFlagPtr);

void updateVoltageAlarm(float* voltageAlarmPtr, bool* voltageAlarmFlag, float* voltageReading, bool *acknowledgeFlagPtr);

#endif

#ifdef __cplusplus 
} 
#endif 
