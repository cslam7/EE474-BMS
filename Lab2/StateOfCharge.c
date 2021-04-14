#include <stdlib.h>
#include <stdbool.h>
#include "StateOfCharge.h"

float socStates[NUMSTATES] = {0, 50, 100};
void updateSOC(float* socPtr, bool* socFlag, int* counterPtr) {
    /****************
    * Function name:            updateSOC
    * Function inputs:          Pointer to present soc value, old soc update flag
    *                           time sync counter
    * Function outputs:         Updated soc value and flag
    * Function description:     Shifts soc value every function call and sets
    *                           update flag as true
    * Author(s): 
    *****************/

   /*
    if soc value are certain values, rotate through array
    update counter and flag = true if 1s has passed
   */
    //float counter = *counterPtr;
    float currSoc = *socPtr;
    if (currSoc == socStates[0]) {
        *socPtr = socStates[1];
    } else if (currSoc == socStates[1]) {
        *socPtr = socStates[2];
    } else {
        *socPtr = socStates[0];
    }

    if (currSoc != *socPtr) {
        *socFlag = TRUE;
    } else {
        *socFlag = FALSE;
    }
    return;
}

void socTask(void* mData) {
    /****************
    * Function name:            socTask
    * Function inputs:          Pointer to struct that holds present soc data
    * Function outputs:         Calls updateSOC
    * Function description:     Casts data to appropriate type and call updates
    * Author(s): 
    *****************/
  	socData* data = (socData*) mData;
    
  	// Update all sensors
  	updateSOC(data->soc, data->socFlag, data->timeCount);
  	
	return;
}
