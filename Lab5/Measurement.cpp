#include "Measurement.h"

extern volatile float hvilAlarm;                     // set initial state for hvil alarm
extern volatile bool contactState;                   // set initial state for the contactor
extern volatile bool oldContact;                     // value of old contactor state 
extern volatile bool toggleFlag;                     // will not toggle contactor state on start up
extern volatile byte contactPin;                     // output pin for contactor
extern volatile bool hvilAlarmFlag;                  // flag for updating hvil alarm
extern volatile bool acknowledgeFlag;                // flag for there is an active not unacknowledged alarm
extern volatile bool acknowledgeButtonExistsFlag;    // flag for the acknowledge button existing

extern volatile int counter;

// interrupt service routine for HVIL circuit
void hvilISR() {
    pinMode(contactPin, OUTPUT);
    contactState = OPEN;
    oldContact = CLOSE;
    digitalWrite(contactPin, !contactState);
//    toggleFlag = TRUE;
    hvilAlarm = STATE2;
    hvilAlarmFlag = TRUE;
    acknowledgeFlag = TRUE;
    // counter++;
}

void updateHVIL(bool* hvilReading, const byte* hvilPin, bool* hvilFlagPtr, bool *acknowledgeFlag) {

    /****************
    * Function name:            updateHVIL
    * Function inputs:          Pointer to GPIO on pin 21 as well as global variable and update display flags
    * Function outputs:         Updated HVIL variable to be displayed
    * Function description:     Shifts HVIL value depending on input pin per scheduler run
    * Author(s): 
    *****************/

    bool oldReading = *hvilReading;
//    Serial.println(*hvilReading);
    pinMode(*hvilPin, INPUT_PULLUP);
    *hvilReading = digitalRead(*hvilPin);
    if (*hvilReading == TRUE) {
        *acknowledgeFlag = FALSE;
    }
    if (oldReading != *hvilReading) {
        *hvilFlagPtr = TRUE;
    } else {
        *hvilFlagPtr = FALSE;
    }
    
    // if there is danger, then go into interrupt and set alarm
    attachInterrupt(digitalPinToInterrupt(*hvilPin), hvilISR, RISING);
    return;
}

void updateTemperature(float* temperatureReading, bool* tempFlag, const byte *tempPin,
                       float *minTempPtr, bool *minTempFlagPtr, float *maxTempPtr,
                       bool *maxTempFlagPtr, bool *resetFlagPtr) {

    /****************
    * Function name:            updateTemperature
    * Function inputs:          Pointer to present temperature value, old temperature update flag,
    *                           and temperature GPIO pin. Also includes historical min/max variables
    * Function outputs:         Updated temperature value and flag
    * Function description:     Shifts temperature value every 1s function call and sets
    *                           update flag if true
    * Author(s): 
    *****************/

    float presentTemp = *temperatureReading;
    pinMode(*tempPin, INPUT_PULLUP);
    *temperatureReading = (55.0 * analogRead(*tempPin) / 1023) - 10;

    if (presentTemp != *temperatureReading) {
        *tempFlag = TRUE;
    } else {
        *tempFlag = FALSE;
    }


    // if reset is true, set min/max to 0
    if (*resetFlagPtr == TRUE) {
        *minTempPtr = *temperatureReading;
        *minTempFlagPtr = TRUE;
        *maxTempPtr = *temperatureReading;
        *maxTempFlagPtr = TRUE;
    }

    // update min/max accordingly if present values more extreme than historical
    if (*temperatureReading < *minTempPtr) {
        *minTempPtr = *temperatureReading;
        *minTempFlagPtr = TRUE;
    } else if (*temperatureReading > *maxTempPtr) {
        *maxTempPtr = *temperatureReading;
        *maxTempFlagPtr = TRUE;
    }
    return;  
}

void updateHvCurrent(float* currentReading, bool* currentFlag, const byte *currentPin,
                     float *minCurrentPtr, bool *minCurrentFlagPtr, float *maxCurrentPtr,
                     bool *maxCurrentFlagPtr, bool *resetFlagPtr) {
    /****************
    * Function name:            updateHvCurrent
    * Function inputs:          Pointer to present current value, old current update flag
    *                           and current GPIO pin .Also includes historical min/max variables
    * Function outputs:         Updated current value and flag
    * Function description:     Shifts current value every 2s function call and sets
    *                           update flag if true
    * Author(s): 
    *****************/
    float presentCurrent = *currentReading;
    pinMode(*currentPin, INPUT_PULLUP);
    *currentReading = (50.0 * analogRead(*currentPin) / 1023) - 25;

    if (presentCurrent != *currentReading) {
        *currentFlag = TRUE;
    } else {
        *currentFlag = FALSE;
    }

    // if reset is true, set min/max to 0
    if (*resetFlagPtr == TRUE) {
        *minCurrentPtr = *currentReading;
        *minCurrentFlagPtr = TRUE;
        *maxCurrentPtr = *currentReading;
        *maxCurrentFlagPtr = TRUE;
    }

    // update min/max accordingly if present values more extreme than historical
//    Serial.print("current reading is"); Serial.println(*currentReading);
//    Serial.print("min current reading is"); Serial.println(*minCurrentPtr);
    if (*currentReading < *minCurrentPtr) {
        *minCurrentPtr = *currentReading;
        *minCurrentFlagPtr = TRUE;
    } else if (*currentReading > *maxCurrentPtr) {
        *maxCurrentPtr = *currentReading;
        *maxCurrentFlagPtr = TRUE;
    }
    return;
} 

void updateHvVoltage(float* voltageReading, bool* voltageFlag, const byte *voltagePin,
                     float *minVoltagePtr, bool *minVoltageFlagPtr, float *maxVoltagePtr,
                     bool *maxVoltageFlagPtr, bool *resetFlagPtr) {
    /****************
    * Function name:            updateHvVoltage
    * Function inputs:          Pointer to present voltage value, old voltage update flag
    *                           and GPIO pin. Also includes historical min/max variables
    * Function outputs:         Updated voltage value and flag
    * Function description:     Shifts voltage value if needed
    * Author(s): 
    *****************/

    float presentVoltage = *voltageReading;
    pinMode(*voltagePin, INPUT_PULLUP);
    *voltageReading = (450.0 * analogRead(*voltagePin) / 1023);
    if (presentVoltage != *voltageReading) {
        *voltageFlag = TRUE;
    } else {
        *voltageFlag = FALSE;
    }

    // if reset is true, set min/max to 0
    if (*resetFlagPtr == TRUE) {
        *minVoltagePtr = *voltageReading;
        *minVoltageFlagPtr = TRUE;
        *maxVoltagePtr = *voltageReading;
        *maxVoltageFlagPtr = TRUE;
//        *resetFlagPtr = FALSE;
    }

    // update min/max accordingly if present values more extreme than historical
    if (*voltageReading < *minVoltagePtr) {
        *minVoltagePtr = *voltageReading;
        *minVoltageFlagPtr = TRUE;
    } else if (*voltageReading > *maxVoltagePtr) {
        *maxVoltagePtr = *voltageReading;
        *maxVoltageFlagPtr = TRUE;
    }
    return;
}

void measurementTask(void* mData) {
    /****************
    * Function name: 			measurementTask
    * Function inputs: 			measure data
    * Function outputs: 		calling function to cycle through measure data
    * Function description: 	cycles through measure and flag variables
    * Author(s): 
    *****************/

  	measurementData* data = (measurementData*) mData;
    
  	// Update all sensors
  	updateHVIL(data->hvilStatus, data->hvilPin, data->hvilFlag, data->acknowledgeFlag);
  	updateTemperature(data->temperature, data->tempFlag, data->tempPin, data->minTemp,
                    data->minTempFlag, data->maxTemp, data->maxTempFlag, data->resetFlag);
  	updateHvCurrent(data->hvCurrent, data->currentFlag, data->currentPin, data->minCurrent,
                    data->minCurrentFlag, data->maxCurrent, data->maxCurrentFlag, data->resetFlag);
  	updateHvVoltage(data->hvVoltage, data->voltageFlag, data->voltagePin, data->minVoltage,
                    data->minVoltageFlag, data->maxVoltage, data->maxVoltageFlag, data->resetFlag);
	return;
}
