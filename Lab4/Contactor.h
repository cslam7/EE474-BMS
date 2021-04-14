
#ifndef Contactor_H_
#define Contactor_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>

#define TRUE 1
#define FALSE 0

#define STATE1 0
#define STATE2 1
#define STATE3 2

#define OPEN 1
#define CLOSE 0

#define LED_ON 1
#define LED_OFF 0

typedef struct contactorTaskData {
	volatile bool* contactState;         // pointer to current state of each alarm
    volatile bool* toggleFlag;           // pointer signaling soc has updated
    volatile byte* contactPin;           // output pin for contactor
    volatile float* hvilAlarm;           // current state of hvilAlarm
    float* currentAlarm;                 // current state of currentAlarm
    float* voltageAlarm;                 // current state of voltageAlarm
    volatile bool *acknowledgeFlag;      // if any alarm is active
    bool* historicalToggleFlag;
    
} contactorData;

void contactorTask (void* mData);

void updateContactor(bool* contactStatePtr, bool* toggleFlag,const byte *contactPin, float *hvilAlarmPtr,
                     float *currentAlarmPtr, float *voltageAlarmPtr, bool *acknowledgeFlagPtr, bool *historicalToggleFlag);
#endif
