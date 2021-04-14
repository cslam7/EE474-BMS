#include <stdlib.h>
#include <stdbool.h>
#include "Contactor.h"

const byte contactPin  = 30;

void updateContactor(bool* contactStatePtr, bool* toggleFlag) {
    /****************
    * Function name:            updateContactor
    * Function inputs:          Pointer to current contactor state and toggle flag
    *                           from display
    * Function outputs:         Updated contactor state and flag -> changes LED output
    * Function description:     Toggles contactor state depending on reponse to touch input
    * Author(s): 
    *****************/

   /*
    if toggleFlag is true, then switch the state of contactor
    otherwise keep the state of the contactor the same
    once complete, turn toggle flag off
   */
    if (*toggleFlag) {
        if (*contactStatePtr) {
            //turn LED on
            digitalWrite(contactPin, HIGH);
            *contactStatePtr = FALSE;
        } else {
            //turn LED off
            digitalWrite(contactPin, LOW);
            *contactStatePtr = TRUE;
        }
    }

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
  	updateContactor(data->contactState, data->toggleFlag);
  	
	return;
}
