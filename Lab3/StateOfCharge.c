#include <stdlib.h>
#include <stdbool.h>
#include "StateOfCharge.h"


void updateSOC(bool* socFlagPtr) {
    /****************
    * Function name:            updateSOC
    * Function inputs:          No inputs
    * Function outputs:         Nothing
    * Function description:     Sets flag for updating SOC to be off
    * Author(s): 
    *****************/

    *socFlagPtr = FALSE;
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
    
  	// Update existing SOC flags and values
  	updateSOC(data->socFlag);
  	
	return;
}
