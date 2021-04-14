#include <stdlib.h>
#include <stdbool.h>
#include "Alarm.h"

float alarmStates[NUMSTATES] = {STATE1, STATE2, STATE3};

void updateHVILAlarm(float* hvilAlarmPtr, bool* hvilAlarmFlag, int* counterPtr) {
    /****************
    * Function name:            updateHVILAlarm
    * Function inputs:          Pointer to present HVIL alarm value, old HVIL alarm update flag,
    *                           time sync counter
    * Function outputs:         Updated HVIL alarm value and flag
    * Function description:     Shifts HVIL value every 1s function call and sets
    *                           update flag if true
    * Author(s): 
    *****************/

   /*
    if HVIL alarm value are certain values, rotate through array
     and flag = true if 1s has passed
   */
   
    float presentHVILAlarm = *hvilAlarmPtr;
    if (presentHVILAlarm == alarmStates[STATE1]) {
        *hvilAlarmPtr = alarmStates[STATE2];
    } else if (presentHVILAlarm == alarmStates[STATE2]) {
        *hvilAlarmPtr = alarmStates[STATE3];
    } else {
        *hvilAlarmPtr = alarmStates[STATE1];
    }

    if (presentHVILAlarm != *hvilAlarmPtr) {
        *hvilAlarmFlag = TRUE;
    } else {
        *hvilAlarmFlag = FALSE;
    }
    return;
}

void updateCurrentAlarm(float* currentAlarmPtr, bool* currentAlarmFlag, int* counterPtr) {
    /****************
    * Function name:            updateCurrentAlarm
    * Function inputs:          Pointer to present current alarm value, old current alarm update flag,
    *                           time sync counter
    * Function outputs:         Updated current alarm value and flag
    * Function description:     Shifts current value every 2s function call and sets
    *                           update flag if true
    * Author(s): 
    *****************/

   /*
    if current alarm value are certain values, rotate through array
    and flag = true if 2s has passed
   */
   
    int counter = *counterPtr;

    if (counter % CURRTIMER == 0) {
        float presentCurrentAlarm = *currentAlarmPtr;
        if (presentCurrentAlarm == alarmStates[STATE1]) {
            *currentAlarmPtr = alarmStates[STATE2];
        } else if (presentCurrentAlarm == alarmStates[STATE2]) {
            *currentAlarmPtr = alarmStates[STATE3];
        } else {
            *currentAlarmPtr = alarmStates[STATE1];
        }

        if (presentCurrentAlarm != *currentAlarmPtr) {
            *currentAlarmFlag = TRUE;
        } else {
            *currentAlarmFlag = FALSE;
        }
    }
    return;
}

void updateVoltageAlarm(float* voltageAlarmPtr, bool* voltageAlarmFlag, int* counterPtr) {
    /****************
    * Function name:            updatevoltageAlarm
    * Function inputs:          Pointer to present voltage alarm value, old voltage alarm update flag
    *                           time sync counter
    * Function outputs:         Updated voltage alarm value and flag
    * Function description:     Shifts voltage value every 3s function call and sets
    *                           update flag if true
    * Author(s): 
    *****************/

   /*
    if voltage alarm value are certain values, rotate through array
    and flag = true if 3s has passed
   */
   
    int counter = *counterPtr;
    if (counter % VOLTTIMER == 0) {
        float presentvoltageAlarm = *voltageAlarmPtr;
        if (presentvoltageAlarm == alarmStates[STATE1]) {
            *voltageAlarmPtr = alarmStates[STATE2];
        } else if (presentvoltageAlarm == alarmStates[STATE2]) {
            *voltageAlarmPtr = alarmStates[STATE3];
        } else {
            *voltageAlarmPtr = alarmStates[STATE1];
        }

        if (presentvoltageAlarm != *voltageAlarmPtr) {
            *voltageAlarmFlag = TRUE;
        } else {
            *voltageAlarmFlag = FALSE;
        }
    }
    return;
}


void alarmTask(void* mData) {
    /****************
    * Function name:            alarmTask
    * Function inputs:          Pointer to struct that holds alarm data
    * Function outputs:         Calls three functions that updates the alarm variables
    * Function description:     Updates 3 variables for alarm states
    * Author(s): 
    *****************/
  	alarmData* data = (alarmData*) mData;
    
  	// Update all sensors
  	updateHVILAlarm(data->hvilAlarm, data->hvilAlarmFlag, data->timeCount);
  	updateCurrentAlarm(data->currentAlarm, data->currentAlarmFlag, data->timeCount);
  	updateVoltageAlarm(data->voltageAlarm, data->voltageAlarmFlag, data->timeCount);
  	
	return;
}
