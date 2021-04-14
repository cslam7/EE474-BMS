#include "Alarm.h"

static float presentHVILAlarm;       // existing hvil state
static float presentCurrentAlarm;    // existing current alarm state
static float presentVoltageAlarm;    // existing voltage state

void updateHVILAlarm(float* hvilAlarmPtr, bool* hvilAlarmFlag, bool *hvilReading) {
    /****************
    * Function name:            updateHVILAlarm
    * Function inputs:          Pointer to present HVIL alarm value, old HVIL alarm update flag,
    *                           and present HVIL value
    * Function outputs:         Updated HVIL alarm value and flag
    * Function description:     Updates HVIL alarm if necessary
    * Author(s): 
    *****************/

    if (*hvilReading == HVIL_OPEN) {                                    // TRUE  when HVIL IS TRUE AND LED IS OFF
        *hvilAlarmPtr = STATE1;
    }
//    Serial.print("old   "); Serial.println(presentHVILAlarm);
//    Serial.print("new   "); Serial.println(*hvilAlarmPtr);

    // conversion from state 1 to other states is handled by hvil interrupt and touch input
    if (presentHVILAlarm != *hvilAlarmPtr) {
        *hvilAlarmFlag = TRUE;
    }
    presentHVILAlarm = *hvilAlarmPtr;
    return;
}

void updateCurrentAlarm(float* currentAlarmPtr, bool* currentAlarmFlag, float *currentReading, bool* acknowledgeFlagPtr) {
    /****************
    * Function name:            updateCurrentAlarm
    * Function inputs:          Pointer to present current alarm value, current alarm update flag,
    *                           and present current value, and unacknowledged alarm
    * Function outputs:         Updated current alarm value, update flag, and unacknowledged active flag
    * Function description:     Updates current alarm if current value is out of designated range and active unacknowledged flag
    * Author(s): 
    *****************/

    
    if ((*currentReading > -5.0) && (*currentReading < 20.0)) {
        *currentAlarmPtr = STATE1;
    } else if (*currentAlarmPtr != STATE3) {
        *currentAlarmPtr = STATE2;
        *acknowledgeFlagPtr = TRUE;
    }
    if (presentCurrentAlarm != *currentAlarmPtr) {
        *currentAlarmFlag = TRUE;
    }

    presentCurrentAlarm = *currentAlarmPtr;
    return;
}

void updateVoltageAlarm(float* voltageAlarmPtr, bool* voltageAlarmFlag, float* voltageReading, bool* acknowledgeFlagPtr) {
    /****************
    * Function name:            updateCurrentAlarm
    * Function inputs:          Pointer to present voltage alarm value, voltage alarm update flag,
    *                           and present voltage value, and unacknowledged alarm
    * Function outputs:         Updated voltage alarm value, update flag, and unacknowledged active flag
    * Function description:     Updates voltage alarm if current value is out of designated range and active unacknowledged flag
    * Author(s): 
    *****************/

    if ((*voltageReading > 280.0) && (*voltageReading < 405.0)){
        *voltageAlarmPtr = STATE1;
    } else if (*voltageAlarmPtr != STATE3) {
        *voltageAlarmPtr = STATE2;
        *acknowledgeFlagPtr = TRUE;
    }
    if (presentVoltageAlarm != *voltageAlarmPtr) {
        *voltageAlarmFlag = TRUE;
    }
    presentVoltageAlarm = *voltageAlarmPtr;
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
  	updateHVILAlarm(data->hvilAlarm, data->hvilAlarmFlag, data->HVIL);
  	updateCurrentAlarm(data->currentAlarm, data->currentAlarmFlag, data->hvCurrent, data->acknowledgeFlag);
  	updateVoltageAlarm(data->voltageAlarm, data->voltageAlarmFlag, data->hvVoltage, data->acknowledgeFlag);

    // if any alarm is active and unacknowledged, set flags accordingly
    if ((*(data->hvilAlarm) != STATE2) && (*(data->currentAlarm) != STATE2) && (*(data->voltageAlarm) != STATE2)) {
        *(data->acknowledgeFlag) = FALSE;
        *(data->acknowledgeButtonExistsFlag) = TRUE;
    }
	return;
}
