#include <stdlib.h>
#include <stdbool.h>
#include "Measurement.h"

void updateHVIL(bool* hvilReading, const byte* pin, bool* hvilFlagPtr) {
    /****************
    * Function name:            updateHVIL
    * Function inputs:          Pointer to GIPO on pin 22
    * Function outputs:         Updated HVIL vaariable to be displays
    * Function description:     Shifts HVIL value depending on input pin per scheduler run
    * Author(s): 
    *****************/
    
    pinMode(*pin, INPUT);
    int pinValue = digitalRead(*pin);
    
    // If HVIL is False then its gonna stay OPEN
    // otherwise it'll close
	bool oldReading = *hvilReading;
    if (pinValue == OFF){
        *hvilReading = HVIL_OPEN;
    } else {
        *hvilReading = HVIL_CLOSED;
    }
    if (oldReading != *hvilReading) {
        *hvilFlagPtr = TRUE;
    } else {
        *hvilFlagPtr = FALSE;
    }

    return;
}

float temp[NUMSTATES] = {-10, 5, 25};
void updateTemperature(float* temperatureReading, bool* tempFlag, int* counterPtr) {

    /****************
    * Function name:            updateTemperature
    * Function inputs:          Pointer to present temperature  value, old temperature update flag
    *                           time sync counter
    * Function outputs:         Updated temperature value and flag
    * Function description:     Shifts temperature value every 1s function call and sets
    *                           update flag if true
    * Author(s): 
    *****************/
    // rotates between temp values
    float presentTemp = *temperatureReading;
    if (presentTemp == temp[STATE1]) {
        *temperatureReading = temp[STATE2];
    } else if (presentTemp == temp[STATE2]) {
        *temperatureReading = temp[STATE3];
    } else {
        *temperatureReading = temp[STATE1];
    }

    if (presentTemp != *temperatureReading) {
        *tempFlag = TRUE;
    } else {
        *tempFlag = FALSE;
    }
    return;
    
}
float current[NUMSTATES] = {-20, 0, 20};
void updateHvCurrent(float* currentReading, bool* currentFlag, int* counterPtr) {
    /****************
    * Function name:            updateHvCurrent
    * Function inputs:          Pointer to present current  value, old current update flag
    *                           time sync counter
    * Function outputs:         Updated current value and flag
    * Function description:     Shifts current value every 2s function call and sets
    *                           update flag if true
    * Author(s): 
    *****************/
    // rotates through current values based on timer
    int counter = *counterPtr;

    if (counter % CURRTIMER == 0) {
        float presentCurrent = *currentReading;
        if (presentCurrent == current[STATE1]) {
            *currentReading = current[STATE2];
        } else if (presentCurrent == current[STATE2]) {
            *currentReading = current[STATE3];
        } else {
            *currentReading = current[STATE1];
        }

        if (presentCurrent != *currentReading) {
            *currentFlag = TRUE;
        } else {
            *currentFlag = FALSE;
        }
    }
    return;
}

float voltage[NUMSTATES] = {10,150,450};
void updateHvVoltage(float* voltageReading, bool* voltageFlag, int* counterPtr) {
    /****************
    * Function name:            updateHvVoltage
    * Function inputs:          Pointer to present voltage  value, old voltage update flag
    *                           time sync counter
    * Function outputs:         Updated voltage value and flag
    * Function description:     Shifts voltage value every 3s function call and sets
    *                           update flag if true
    * Author(s): 
    *****************/

    //rotates through voltage values based on timer
    int counter = *counterPtr;

    if (counter % VOLTTIMER == 0) {
        float presentVoltage = *voltageReading;
        if (presentVoltage == voltage[STATE1]) {
            *voltageReading = voltage[STATE2];
        } else if (presentVoltage == voltage[STATE2]) {
            *voltageReading = voltage[STATE3];
        } else {
            *voltageReading = voltage[STATE1];
        }

        if (presentVoltage != *voltageReading) {
            *voltageFlag = TRUE;
        } else {
            *voltageFlag = FALSE;
        }
    }
    return;
}

void measurementTask(void* mData) {
    /****************
    * Function name: 			measurementTask
    * Function inputs: 			array of measure data
    * Function outputs: 		calling function to cycle through measure data
    * Function description: 	cycles through measure and flag variables
    * Author(s): 
    *****************/


  	measurementData* data = (measurementData*) mData;
    
  	// Update all sensors
  	updateHVIL(data->hvilStatus, data->hvilPin, data->hvilFlag);
  	updateTemperature(data->temperature, data->tempFlag, data->timeCount);
  	updateHvCurrent(data->hvCurrent, data->currentFlag, data->timeCount);
  	updateHvVoltage(data->hvVoltage, data->voltageFlag, data->timeCount);
  	
	return;
}
