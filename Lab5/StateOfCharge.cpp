#include "StateOfCharge.h"

static float tempArray[NUM_TEMP] = {-10, 0, 25, 45};                // given temperature array of data table
static float voltArray[NUM_VOLT] = {200, 250, 300, 350, 400};       // given voltage array of data table
static float dataArray[NUM_TEMP][NUM_VOLT] = {                      // data table via 2-D array
                                        {0, 10, 35, 100, 100},
                                        {0, 0, 20, 80, 100},
                                        {0, 0, 10, 60, 100},
                                        {0, 0, 0, 50, 100}
                                      };

// returns index of boundaries regarding passed value and given array
void findBoundaries(float *presentValue, float boundArray[], int *lowerBoundIndex,
                    int *upperBoundIndex, int size) {
    int i;
    for (i = 0; i < size - 1; i++) {
        if ((*presentValue >= boundArray[i]) && (*presentValue <= boundArray[i+1])) {
            *lowerBoundIndex = i;
            *upperBoundIndex = i + 1;
            return;
        }
    }
}

// performs a 1-D interpolation on given values
float interpolate_segment(float x0, float y0, float x1, float y1, float x) {
    float t;

    t =  (x - x0);
    t /= (x1 - x0);

    return y0 + t*(y1-y0);
}


void calculateSOC(float *socPtr, bool *socFlagPtr, float *temperaturePtr, float *hvCurrentPtr, 
               float *hvVoltagePtr) {
    /****************
    * Function name:            calculateSOC
    * Function inputs:          state of charge, temperature, current, and voltage with update flag
    * Function outputs:         updated SOC value
    * Function description:     Performs 2-D interpolation given measured temperature and voltage values
    *                           to find the state of charge
    * Author(s): 
    *****************/

    float tempBounds[BOUND_SIZE] = {NULL};
    float voltBounds[BOUND_SIZE] = {NULL};
    float volt1;
    float volt2;
    // calculate open circuit voltage
    float Voc = *hvVoltagePtr + (*hvCurrentPtr * RBATT);
    float temp = *temperaturePtr;
    float soc;
    int lowerVoltIndex;
    int lowerTempIndex;
    int upperVoltIndex;
    int upperTempIndex;

    // check for boundaries using for loops
    findBoundaries(&temp, tempArray, &lowerTempIndex, &upperTempIndex, NUM_TEMP);
    tempBounds[UPPER] = tempArray[upperTempIndex];
    tempBounds[LOWER] = tempArray[lowerTempIndex];
//    Serial.print("low temp "); Serial.println(lowerTempIndex);
//    Serial.print("high temp "); Serial.println(upperTempIndex);
    
    findBoundaries(&Voc, voltArray, &lowerVoltIndex, &upperVoltIndex, NUM_VOLT);
    voltBounds[UPPER] = voltArray[upperVoltIndex];
    voltBounds[LOWER] = voltArray[lowerVoltIndex];
//    Serial.print("low volt "); Serial.println(voltBounds[LOWER]);
//    Serial.print("high volt "); Serial.println(voltBounds[UPPER]);

    volt1 = interpolate_segment(tempBounds[LOWER], dataArray[lowerTempIndex][lowerVoltIndex],
                                tempBounds[UPPER], dataArray[upperTempIndex][lowerVoltIndex],
                                temp);

    volt2 = interpolate_segment(tempBounds[LOWER], dataArray[lowerTempIndex][upperVoltIndex],
                                tempBounds[UPPER], dataArray[upperTempIndex][upperVoltIndex],
                                temp);

    soc = interpolate_segment(voltBounds[LOWER], volt1, voltBounds[UPPER], volt2, Voc);

    *socPtr = soc;
    return;
}

void updateSOC(float *socPtr, bool *socFlagPtr, float *temperaturePtr,
               bool *tempFlagPtr, float *hvCurrentPtr, bool *currentFlagPtr, float *hvVoltagePtr,
               bool *voltageFlagPtr) {
    /****************
    * Function name:            updateSOC
    * Function inputs:          No inputs
    * Function outputs:         Nothing
    * Function description:     Sets flag for updating SOC to be off
    * Author(s): 
    *****************/

   // if the current, voltage, or temperature has changed, recalculate SOC
   if (*tempFlagPtr || *currentFlagPtr || *voltageFlagPtr) {
       calculateSOC(socPtr, socFlagPtr, temperaturePtr, hvCurrentPtr, 
               hvVoltagePtr);
       *socFlagPtr = TRUE;
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
    
  	// Update existing SOC flags and values
  	updateSOC(data->soc, data->socFlag, data->temperature,
              data->tempFlag, data->hvCurrent, data->currentFlag, data->hvVoltage,
              data->voltageFlag);
  	
	return;
}
