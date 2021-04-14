#include <stdlib.h>
#include <stdbool.h>
#include "Contactor.h"
void updateContactor(bool* contactStatePtr, bool* toggleFlag, float* hvilAlarmPtr, const byte *contactPin, bool *hvilReading) {
    /****************
    * Function name:            updateContactor
    * Function inputs:          Pointer to current contactor state and toggle flag
    *                           from display
    * Function outputs:         Updated contactor state and flag -> changes LED output
    * Function description:     Toggles contactor state depending on reponse to touch input
    * Author(s): 
    *****************/

    // contactor's critical code section to avoid conflicts with HVIL interrupt
    noInterrupts();
    if (*hvilReading == FALSE) {            // LED IS ON for HVIL
        if (*toggleFlag && (*hvilAlarmPtr == STATE1) && *contactStatePtr) {     // if toggle is pressed while contactor LED is off, turn contactor LED on
            digitalWrite(*contactPin, LED_ON);
            *contactStatePtr = CLOSE;
        } else if (((*hvilAlarmPtr == STATE2) || (*hvilAlarmPtr == STATE3)) || (*toggleFlag && !*contactStatePtr)) {    // if toggle is pressed while contactor LED is on, turn contactor LED on
            digitalWrite(*contactPin, LED_OFF);
            *contactStatePtr = OPEN;
        }
    } else {                                // LED is OFF for HVIL
        digitalWrite(*contactPin, CLOSE);
        *contactStatePtr = OPEN;
        *toggleFlag = FALSE;
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
  	updateContactor(data->contactState, data->toggleFlag, data->hvilAlarm, data->contactPin, data->hvilReading);
  	
	return;
}
