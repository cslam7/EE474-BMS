#include "Contactor.h"

void updateContactor(bool* contactStatePtr, bool* toggleFlag, const byte *contactPin, float *hvilAlarmPtr,
                     float *currentAlarmPtr, float *voltageAlarmPtr, bool *acknowledgeFlagPtr, bool *historicalToggleFlag) {
    /****************
    * Function name:            updateContactor
    * Function inputs:          Pointer to current contactor state and toggle flag
    *                           from display
    * Function outputs:         Updated contactor state and flag -> changes LED output
    * Function description:     Toggles contactor state depending on response to touch input
    * Author(s): 
    *****************/

    // contactor's critical code section to avoid conflicts with HVIL interrupt
//    Serial.println(*hvilAlarmPtr);
//    Serial.println(*currentAlarmPtr);
//    Serial.println(*voltageAlarmPtr);
//    Serial.println(*toggleFlag);
//    Serial.println(*contactStatePtr);
    noInterrupts();
    if ((*hvilAlarmPtr == STATE1) && (*currentAlarmPtr == STATE1) && (*voltageAlarmPtr == STATE1)) {     // all alarms are not active
        if (*toggleFlag && *contactStatePtr) {            // if toggle is pressed while contactor LED is off, turn contactor LED on
            digitalWrite(*contactPin, LED_ON);
            *contactStatePtr = CLOSE;
//            Serial.println("IM TURNING ON");
        } else if (*toggleFlag && !*contactStatePtr) {    // if toggle is pressed while contactor LED is on, turn contactor LED on
            digitalWrite(*contactPin, LED_OFF);
            *contactStatePtr = OPEN;
//            Serial.println("I'M TURNING OFF");
        }
         *toggleFlag = FALSE;
         *historicalToggleFlag = TRUE;
    } else {                                // if any alarm is active
        digitalWrite(*contactPin, CLOSE);
        *contactStatePtr = OPEN;
    }
    interrupts();

    return;
}

void contactorTask(void* mData) {
    /****************
    * Function name:            contactorTask
    * Function inputs:          Pointer to contactor state and touch input
    * Function outputs:         Calls function to update contactor state
    * Function description:     Toggles contactor state depdning on touch input
    * Author(s): 
    *****************/
  	contactorData* data = (contactorData*) mData;
    
  	// Update all sensors
  	updateContactor(data->contactState, data->toggleFlag, data->contactPin, data->hvilAlarm,
  	                data->currentAlarm, data->voltageAlarm, data->acknowledgeFlag, data->historicalToggleFlag);
  	
	return;
}
