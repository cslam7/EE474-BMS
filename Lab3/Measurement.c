#include <stdlib.h>
#include <stdbool.h>
#include "Measurement.h"

extern volatile float hvilAlarm;                     // set initial state for hvil alarm

extern volatile bool contactState;                   // set initial state for the contactor
extern volatile bool oldContact;                     // value of old contactor state 
extern volatile bool toggleFlag;                     // will not toggle contactor state on start up
extern volatile byte contactPin;                     // output pin for contactor
extern volatile bool hvilAlarmFlag;                       // flag for updating hvil alarm
extern volatile bool acknowledgeFlag;                // flag for there is an active not unacknowledged alarm
extern volatile bool acknowledgeButtonExistsFlag;    // flag for the acknowledge button existing

extern volatile int counter;

// interrupt service routine for HVIL circuit
void hvilISR() {
    pinMode(contactPin, OUTPUT);
    contactState = OPEN;
    oldContact = CLOSE;
    digitalWrite(contactPin, !contactState);
    toggleFlag = TRUE;
    hvilAlarm = STATE2;
    hvilAlarmFlag = TRUE;
    acknowledgeFlag = TRUE;
    // counter++;
}

void updateHVIL(bool* hvilReading, const byte* hvilPin, bool* hvilFlagPtr, bool *acknowledgeFlag) {
    /****************
    * Function name:            updateHVIL
    * Function inputs:          Pointer to GPIO on pin 21
    * Function outputs:         Updated HVIL variable to be displayed
    * Function description:     Shifts HVIL value depending on input pin per scheduler run
    * Author(s): 
    *****************/

    bool oldReading = *hvilReading;
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

void updateTemperature(float* temperatureReading, bool* tempFlag, const byte* pin) {

    /****************
    * Function name:            updateTemperature
    * Function inputs:          Pointer to present temperature value, old temperature update flag
    *                           and temperature GPIO pin
    * Function outputs:         Updated temperature value and flag
    * Function description:     Shifts temperature value every 1s function call and sets
    *                           update flag if true
    * Author(s): 
    *****************/

    float presentTemp = *temperatureReading;
    pinMode(*pin, INPUT_PULLUP);
    *temperatureReading = (55.0 * analogRead(*pin) / 1023) - 10;

    if (presentTemp != *temperatureReading) {
        *tempFlag = TRUE;
    } else {
        *tempFlag = FALSE;
    }
    return;  
}

void updateHvCurrent(float* currentReading, bool* currentFlag, const byte* pin) {
    /****************
    * Function name:            updateHvCurrent
    * Function inputs:          Pointer to present current value, old current update flag
    *                           and current GPIO pin
    * Function outputs:         Updated current value and flag
    * Function description:     Shifts current value every 2s function call and sets
    *                           update flag if true
    * Author(s): 
    *****************/

    float presentCurrent = *currentReading;
    pinMode(*pin, INPUT_PULLUP);
    *currentReading = (50.0 * analogRead(*pin) / 1023) - 25;
    if (presentCurrent != *currentReading) {
        *currentFlag = TRUE;
    } else {
        *currentFlag = FALSE;
    }
    return;
} 

void updateHvVoltage(float* voltageReading, bool* voltageFlag, const byte* pin) {
    /****************
    * Function name:            updateHvVoltage
    * Function inputs:          Pointer to present voltage value, old voltage update flag
    *                           and GPIO pin
    * Function outputs:         Updated voltage value and flag
    * Function description:     Shifts voltage value if needed
    * Author(s): 
    *****************/

    float presentVoltage = *voltageReading;
    pinMode(*pin, INPUT_PULLUP);
    *voltageReading = (450.0 * analogRead(*pin) / 1023);
    if (presentVoltage != *voltageReading) {
        *voltageFlag = TRUE;
    } else {
        *voltageFlag = FALSE;
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
  	updateTemperature(data->temperature, data->tempFlag, data->tempPin);
  	updateHvCurrent(data->hvCurrent, data->currentFlag, data->currentPin);
  	updateHvVoltage(data->hvVoltage, data->voltageFlag, data->voltagePin);
  	
	return;
}
