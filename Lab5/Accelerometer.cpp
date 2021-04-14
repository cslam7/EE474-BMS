#include "Accelerometer.h"
extern volatile int global_counter;

static float xOffset = 0.03;
static float yOffset = -0.11;
static float zOffset = 0.38;

static float updateTotalX;
static float updateTotalY;
float integration(float oldValue, float newValue, float width) {
    float base1 = (newValue + oldValue);
    float trapezoidArea = width * base1 / 2.0; 

    return trapezoidArea;
}



// this function calculates the updated average using most recent inputed in maxIndex sized data
float rollingAverage(float input, float *bufferPtr, int currentIndex, int maxIndex) {
    int i = 0;
    float tempSum = 0;
    if ((input >= -0.05) && (input <= 0.05)) {
//        Serial.print("I am getting set to 0  as input is   "); Serial.println(input);
        *(bufferPtr + currentIndex) = 0.0;
    } else {
        *(bufferPtr + currentIndex) = input;
    }
//    *(bufferPtr + currentIndex) = input;
//    if (global_counter % 10 == 0) {
//        Serial.print("the buffer contains");

//    }
    for (i = 0; i < maxIndex; i++) {
        tempSum += *(bufferPtr + i);
//        if (*(bufferPtr + i) == 0.0) {
//            //flag to set true
//        }
//        Serial.println("the buffer c
    }
    float maxIndexAvoidTruncate = (float) maxIndex;
    return tempSum / maxIndexAvoidTruncate;
}

void accelInput(float *inputXPtr, float *inputYPtr, float *inputZPtr, 
                float *oldInputXPtr, float *oldInputYPtr, float *oldInputZPtr, 
                int *bufferIndex, float *bufferX, float *bufferY, float *bufferZ,
                int *bufferFilled, const byte *pinX, const byte *pinY, const byte *pinZ,
                bool *deltaXFlagPtr, bool *deltaYFlagPtr, bool *deltaZFlagPtr) {
                
    /****************
    * Function name:            inputAccel
    * Function inputs:          Pointer to variables that hold g values from analog input
    * Function outputs:         Updated global variables that hold g values
    * Function description:     Update delta variables based on pin inputs using a rolling
    *                           average buffer to 
    * Author(s): 
    *****************/

    // store old values for acceleration in gs
    *oldInputXPtr = *inputXPtr;
    *oldInputYPtr = *inputYPtr;
    *oldInputZPtr = *inputZPtr;
//    if (global_counter % 10 == 0) {
//        Serial.print("x is :"); Serial.println(*oldInputXPtr);
//    }

    // if buffer is filled, then start at beginning of buffer next time;
    if (*bufferIndex == BUFFER_SIZE) {
        *bufferIndex = 0;
    } else if (*bufferFilled != BUFFER_FULL) {  // if buffer has never been filled, then increment max index
        (*bufferFilled)++;
    }
    
    int i = 0;
    *inputXPtr = (1.25 * (5.0 * analogRead(*pinX) / 1023.0) - 2.0625) + xOffset;
    *inputYPtr = (1.25 * (5.0 * analogRead(*pinY) / 1023.0) - 2.0625)+ yOffset;
    *inputZPtr = (1.25 * (5.0 * analogRead(*pinZ) / 1023.0) - 2.0625) + zOffset;
//    if (global_counter % 10 == 0) {

//        Serial.print("x is :"); Serial.print(analogRead(*pinX)); Serial.print("   y is :"); Serial.print(analogRead(*pinY)); Serial.print("   z is :"); Serial.println(analogRead(*pinZ));
//    }

   
    *inputXPtr = rollingAverage(*inputXPtr, bufferX, *bufferIndex, *bufferFilled);
    *inputYPtr = rollingAverage(*inputYPtr, bufferY, *bufferIndex, *bufferFilled);
    *inputZPtr = rollingAverage(*inputZPtr, bufferZ, *bufferIndex, *bufferFilled);
//    Serial.print("buffer index is   "); Serial.println(*bufferIndex);
//    Serial.print("buffer loop max is    "); Serial.println(*bufferFilled);
//    if (global_counter % 10 == 0) {
//        Serial.print("oldX is :"); Serial.println(*oldInputXPtr);
//        Serial.print("newX is :"); Serial.println(*inputXPtr);
//
//    }
//    if (global_counter % 100 == 0) {
//        Serial.print("x is :"); Serial.print(*inputXPtr); Serial.print("   y is :"); Serial.print(*inputYPtr); Serial.print("   z is :"); Serial.println(*inputZPtr);
//    }
    (*bufferIndex)++;

    
    return;
}
                
void updateRelative(float *deltaXPtr, float *deltaYPtr, float *deltaZPtr,
                    float *oldDeltaXPtr, float *oldDeltaYPtr, float *oldDeltaZPtr,
                    float *inputXPtr, float *inputYPtr, float *inputZPtr,
                    float *oldInputXPtr, float *oldInputYPtr, float *oldInputZPtr,
                    float *velocityXPtr, float *velocityYPtr, float *velocityZPtr,
                    float *oldVelocityXPtr, float *oldVelocityYPtr, float *oldVelocityZPtr,
                    const byte *pinX, const byte *pinY, const byte *pinZ,
                    bool *deltaXFlagPtr, bool *deltaYFlagPtr, bool *deltaZFlagPtr,
                    float *timeBasePtr, float *bufferX, float *bufferY, float *bufferZ,
                    int *bufferFilled) {

    /****************
    * Function name:            updateRelative
    * Function inputs:          Pointer to present delta variables, update flags, and input pins
    * Function outputs:         Updated delta variabes and update flags
    * Function description:     Update delta variables based on pin inputs and set update flags if
    *                           value has changed in the last clock cycle
    * Author(s): 
    *****************/
    
    // store old values for deltas
    updateTotalX = *deltaXPtr;
    updateTotalY = *deltaYPtr;
//    *oldDeltaZPtr = *deltaZPtr;

    // store old values for velocity
    *oldVelocityXPtr = *velocityXPtr;
    *oldVelocityYPtr = *velocityYPtr;
    *oldVelocityZPtr = *velocityZPtr;

    int i, zeroVxCount, zeroVyCount, zeroVzCount = 0;
    float oldAccelerationX = *oldInputXPtr * CONVERT_CM;
    float oldAccelerationY = *oldInputYPtr * CONVERT_CM;
    float oldAccelerationZ = (*oldInputZPtr + 1.0) * CONVERT_CM;   
    
    float accelerationX = *inputXPtr * CONVERT_CM;
    float accelerationY = *inputYPtr * CONVERT_CM;
    float accelerationZ = (*inputZPtr + 1.0) * CONVERT_CM;
    
//    Serial.print("xaccel is :"); Serial.print(accelerationX); Serial.print("   yaccel is :"); Serial.print(accelerationY); Serial.print("   zaccel is :"); Serial.println(accelerationZ);
//    if (global_counter % 1 == 0) {
//        Serial.print("oldaccelX is :"); Serial.print(oldAccelerationX); Serial.print("   accelX is :"); Serial.println(accelerationX);
//    }
//    Serial.print("oldvelocityY prior to ovf is :"); Serial.print(*oldVelocityYPtr); Serial.print("   velocityY is :"); Serial.print(*velocityYPtr);
//    float velocityX = *oldVelocityXPtr;
////    Serial.print("oldvelocityX prior to ovf is :"); Serial.print(*oldVelocityXPtr); Serial.print("   velocityX is :"); Serial.print(*velocityXPtr);
//    float diffX = integration(oldAccelerationX, accelerationX, 0.1);
//    *velocityXPtr = velocityX + diffX;
////    Serial.print("velocity before adding diffX is    "); Serial.println(velocityX);
////    Serial.print("diffX is    "); Serial.println(diffX);
////    
////    Serial.print("newVelocity is    "); Serial.println(*velocityXPtr);
//    float diffY = integration(oldAccelerationY, accelerationY, 0.1);
//    *velocityYPtr = *velocityYPtr + diffY;
//    float diffZ = integration(oldAccelerationZ, accelerationZ, 0.1);
//    *velocityZPtr = *velocityZPtr + diffZ;

    *velocityXPtr += integration(oldAccelerationX, accelerationX, *timeBasePtr);
    *velocityYPtr += integration(oldAccelerationY, accelerationY, *timeBasePtr);
    *velocityZPtr += integration(oldAccelerationZ, accelerationZ, *timeBasePtr);
//    float area = 0.1 * (accelerationX - oldAccelerationX) / 2.0;
//    Serial.print("area is    "); Serial.println(area);
//    *velocityXPtr += 0.1 * (accelerationX + oldAccelerationX) / 2.0;
//    *velocityYPtr += 0.1 * (accelerationY + oldAccelerationY) / 2.0;
//    *velocityZPtr += 0.1 * (accelerationZ + oldAccelerationZ) / 2.0;
//    integration(velocityXPtr, oldAccelerationX, accelerationX, 0.1);
//    integration(velocityYPtr, oldAccelerationY, accelerationY, 0.1);
//    integration(velocityZPtr, oldAccelerationZ, accelerationZ, 0.1);

    // if acceleration has stopped, then stop velocity

    // checks through buffers, increments count if 0.0 is detected
    for (i = 0; i < *bufferFilled; i++) {
        if (*(bufferX + i) == 0.0) {
            zeroVxCount++;
        }
        if (*(bufferY + i) == 0.0) {
            zeroVyCount++;
        }
        if (*(bufferZ + i) == 0.0) {
            zeroVzCount++;
        }
    }

//    // if all values are 0.0, then set velocity to 0.0
    if (zeroVxCount == *bufferFilled) {
        *velocityXPtr = 0.0;
    }
    if (zeroVyCount == *bufferFilled) {
        *velocityYPtr = 0.0;
    }
    if (zeroVzCount == *bufferFilled) {
        *velocityZPtr = 0.0;
    }

    if ((*inputXPtr < 0.02) &&( *inputXPtr > -0.02)) {
        *velocityXPtr = 0.0;
    }
    
    if ((*inputYPtr < 0.05) &&( *inputYPtr > -0.05)){
        *velocityYPtr = 0.0;
    }
    if ((*inputZPtr < 0.05) &&( *inputZPtr > -0.05)) {
        *velocityZPtr = 0.0;
    }
    
    
//    if (global_counter % 100 == 0) {
//        Serial.print("oldvelocityY is :"); Serial.print(*oldVelocityYPtr); Serial.print("   velocityY is :"); Serial.println(*velocityYPtr);
//    }
    *deltaXPtr += integration(*oldVelocityXPtr, *velocityXPtr, *timeBasePtr);
    *deltaYPtr += integration(*oldVelocityYPtr, *velocityYPtr, *timeBasePtr);
    *deltaZPtr += integration(*oldVelocityZPtr, *velocityZPtr, *timeBasePtr);
//            *deltaXPtr += 0.1 * (*velocityXPtr + *oldVelocityXPtr) / 2.0;
//            *deltaYPtr += 0.1 * (*velocityYPtr + *oldVelocityYPtr) / 2.0;
//            *deltaZPtr += 0.1 * (*velocityZPtr + *oldVelocityZPtr) / 2.0;
        //    integration(deltaXPtr, *oldVelocityXPtr, *velocityXPtr, 0.1);
        //    integration(deltaYPtr, *oldVelocityYPtr, *velocityYPtr, 0.1);
        //    integration(deltaZPtr, *oldVelocityZPtr, *velocityZPtr, 0.1);
//    if (global_counter % 10 == 0) {
        
    
            //    Serial.print("oldvelocityX is :"); Serial.print(*oldVelocityXPtr); Serial.print("   velocityX is :"); Serial.print(*velocityXPtr); Serial.print("   deltaX is :"); Serial.println(*deltaXPtr);
        Serial.print("x is :"); Serial.print(*inputXPtr); Serial.print("   y is :"); Serial.print(*inputYPtr); Serial.print("   z is :"); Serial.println(*inputZPtr + 1.0);
    
        Serial.print("velX is :"); Serial.print(*velocityXPtr); Serial.print("   velY is :"); Serial.print(*velocityYPtr); Serial.print("   velZ is :"); Serial.println(*velocityZPtr);
        
        Serial.print("deltaX is :"); Serial.print(*deltaXPtr); Serial.print("   deltaY is :"); Serial.print(*deltaYPtr); Serial.print("   deltaZ is :"); Serial.println(*deltaZPtr);
//    }
    if (*oldDeltaXPtr != *deltaXPtr) {
        *deltaXFlagPtr = TRUE;
    }

    if (*oldDeltaYPtr != *deltaYPtr) {
        *deltaYFlagPtr = TRUE;
    }

    if (*oldDeltaZPtr != *deltaZPtr) {
        *deltaZFlagPtr = TRUE;
    }

//    if (global_counter % 100 == 0) {
//        Serial.print("deltax is :"); Serial.print(*deltaXPtr); Serial.print("   deltay is :"); Serial.print(*deltaYPtr); Serial.print("   deltaz is :"); Serial.println(*deltaZPtr);
//    }
    return;  
}

void updateTotal(float *distanceTraveledPtr, float *deltaXPtr, float *deltaYPtr,
                 float *oldDeltaXPtr, float *oldDeltaYPtr, float *oldDistanceTraveledPtr,
                 bool *distanceTraveledFlagPtr, bool *deltaXFlagPtr, bool *deltaYFlagPtr) {
    /****************
    * Function name:            updateTotal
    * Function inputs:          Pointer to present delta varaibles, total distance traveled and update flags
    * Function outputs:         Updated total distance traveled and update flag
    * Function description:     Update total distance traveled based on delta variables and update flag if
    *                           value has changed in the last clock cycle
    * Author(s): 
    *****************/
    float displaceX, displaceY = 0.0;
    if (*deltaXFlagPtr == TRUE) {
        displaceX = fabs(*deltaXPtr - updateTotalX); 
    }
    if (*deltaYFlagPtr == TRUE) {
        displaceY = fabs(*deltaYPtr - updateTotalY);
    }
//    float displaceX = fabs(*deltaXPtr - *oldDeltaXPtr);
//    float displaceY = fabs(*deltaYPtr - *oldDeltaYPtr);
    // float displaceZ = fabs(*deltaZPtr - *oldDeltaZPtr);      // take out since demo is in one plane
//    *oldDistanceTraveledPtr = *distanceTraveledPtr;
    *distanceTraveledPtr += displaceX + displaceY;
    Serial.print("distance traveled is    "); Serial.println(*distanceTraveledPtr);
    if (*oldDistanceTraveledPtr != *distanceTraveledPtr) {
        *distanceTraveledFlagPtr = TRUE;
    }

    return;
} 

void updateDegree(float *degreeXPtr, float *degreeYPtr, float *degreeZPtr,
                  float *oldDegreeXPtr, float *oldDegreeYPtr, float *oldDegreeZPtr,  
                  float *inputXPtr, float *inputYPtr, float *inputZPtr,
                  bool *degreeXFlagPtr, bool *degreeYFlagPtr, bool *degreeZFlagPtr) {
    /****************
    * Function name:            updateDegree
    * Function inputs:          Pointer to present degreevariables, update flags, and input pins
    * Function outputs:         Updated degree variabes and update flags
    * Function description:     Update degree variables based on pin inputs and set update flags if
    *                           value has changed in the last clock cycle
    * Author(s): 
    *****************/
//    *oldDegreeXPtr = *degreeXPtr;
//    *oldDegreeYPtr = *degreeYPtr;
//    *oldDegreeZPtr = *degreeZPtr;

    float magnitude = sqrt((*inputXPtr * *inputXPtr) + (*inputYPtr * *inputYPtr) + (*inputZPtr * *inputZPtr));
//    float PI = 3.141592;
    
    double calculationX = (double)  *inputXPtr / magnitude;
    float radianX = (float) acos(calculationX);
    *degreeXPtr = radianX * (180/PI);

    double calculationY = (double) *inputYPtr / magnitude;
    float radianY = (float) acos(calculationY);
    *degreeYPtr = radianY * (180/PI);

    double calculationZ = (double) *inputZPtr / magnitude;
    float radianZ = (float) acos(calculationZ);
    *degreeZPtr = radianZ * (180/PI);

    if (*oldDegreeXPtr != *degreeXPtr) {
        *degreeXFlagPtr = TRUE;
    }

    if (*oldDegreeYPtr != *degreeYPtr) {
        *degreeYFlagPtr = TRUE;
    }

    if (*oldDegreeZPtr != *degreeZPtr) {
        *degreeZFlagPtr = TRUE;
    }
    return;  
}

void accelerometerTask(void* mData) {
    /****************
    * Function name: 			accelerometerTask
    * Function inputs: 			accelerometer data
    * Function outputs: 		calling function to update accelerometer data
    * Function description: 	update accelerometer varibles from input pins and update flags
    * Author(s): 
    *****************/

  	accelerometerData* data = (accelerometerData*) mData;
    
  	// Update all sensors
    accelInput(data->inputX, data->inputY, data->inputZ,
              data->oldInputX, data->oldInputY, data->oldInputZ,
              data->bufferIndex, data->bufferX, data->bufferY,
              data->bufferZ, data->bufferFilled, data->pinX,
              data->pinY, data->pinZ, data->deltaXFlag,
              data->deltaYFlag, data->deltaZFlag);
 	updateRelative(data->deltaX, data->deltaY, data->deltaZ,
                  data->oldDeltaX, data->oldDeltaY, data->oldDeltaZ,
                  data->inputX, data->inputY, data->inputZ,
                  data->oldInputX, data->oldInputY, data->oldInputZ,
                  data->velocityX, data->velocityY, data->velocityZ,
                  data->oldVelocityX, data->oldVelocityY, data->oldVelocityZ,
                  data->pinX, data->pinY, data->pinZ, data->deltaXFlag, 
                  data->deltaYFlag, data->deltaZFlag, data->timeBase,
                  data->bufferX, data->bufferY, data->bufferZ, data->bufferFilled);
 	updateTotal(data->distanceTraveled, data->deltaX, data->deltaY, 
               data->oldDeltaX, data->oldDeltaY, data->oldDistanceTraveled, 
               data->distanceTraveledFlag, data->deltaXFlag, data->deltaYFlag);
 	updateDegree(data->degreeX, data->degreeY, data->degreeZ,
                 data->oldDegreeX,data->oldDegreeY,data->oldDegreeZ,
                data->inputX, data->inputY, data->inputZ,
                data->degreeXFlag, data->degreeYFlag, data->degreeZFlag);

	return;
}
