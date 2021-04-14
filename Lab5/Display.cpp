#include "Display.h"

// extern volatile int counter;                 // counter for debugging

//converts float to char [] for alarm state
char* convertAlarm(float state) {
    char *allStates[] = {STATE1, STATE2, STATE3};
    return allStates[(int)state];
}

// makes button based on original oritentation
void makeButtons(Elegoo_TFTLCD *tftPtr) {
    
    Elegoo_TFTLCD tft = *tftPtr;
    unsigned long start = micros();
    
    //prev button
    tft.drawRect(10, 255, 100, 50, 0x07FF);
    tft.fillRect(10, 255, 100, 50, 0x001F);
    tft.setCursor(35, 275);
    tft.setTextColor(WHITE); 
    tft.setTextSize(2);
    tft.print("Prev");

    // next button
    tft.drawRect(130, 255, 100, 50, BLUE);
    tft.fillRect(130, 255, 100, 50, BLUE);
    tft.setCursor(155, 275);
    tft.setTextColor(WHITE); 
    tft.setTextSize(2);
    tft.print("Next");
}

// makes acknowledge button on alarm screen
void acknowledgeButton(Elegoo_TFTLCD *tftPtr) {

    Elegoo_TFTLCD tft = *tftPtr;
    unsigned long start = micros();

    tft.drawRect(45, 190, 150, 50, RED);
    tft.fillRect(45, 190, 150, 50, RED);
    tft.setCursor(55,210);
    tft.setTextColor(WHITE); 
    tft.setTextSize(2);
    tft.print("Acknowledge");
}

// clears acknowledge button from alarm screehn
void clearAcknowledgeButton(Elegoo_TFTLCD *tftPtr) {

    Elegoo_TFTLCD tft = *tftPtr;
    unsigned long start = micros();

    tft.drawRect(45, 190, 150, 50, BLACK);
    tft.fillRect(45, 190, 150, 50, BLACK);
}


void updateIntValue(int x, int y, int oldColor, int newColor, int textSize, float *oldValue,
                    float *newValue, bool *updateFlag, Elegoo_TFTLCD *tftPtr) {

    Elegoo_TFTLCD tft = *tftPtr;
    unsigned long start = micros();
    // updates print statements for desired floats
    int temp = int (*newValue);
    tft.setCursor(x, y);
    tft.setTextColor(oldColor);
    tft.setTextSize(textSize);
    tft.println(*oldValue);
    
    tft.setCursor(x, y);
    tft.setTextSize(textSize);
    tft.setTextColor(newColor);
    tft.println(temp);
    *oldValue = temp;
    *updateFlag = FALSE;
    return;

}

void updateCharValue(int x, int y, int oldColor, int newColor, int textSize, char **oldValue,
                     char **newValue, bool *updateFlag, Elegoo_TFTLCD *tftPtr) {

    Elegoo_TFTLCD tft = *tftPtr;
    unsigned long start = micros();
    // updates print statements for desired floats
    tft.setCursor(x, y);
    tft.setTextColor(oldColor);
    tft.setTextSize(textSize);
    tft.println(*oldValue);
    
    tft.setCursor(x, y);
    tft.setTextSize(textSize);
    tft.setTextColor(newColor);
    tft.println(*newValue);
    *oldValue = *newValue;
    *updateFlag = FALSE;
    return;

}

void screenMeasurement(bool *hvilStatus, float *temperature, float *hvCurrent, float *hvVoltage, float *stateCharge,
                       float *oldTemp, float *oldCurr, float *oldVolt, float *oldSoc, float *oldHVIL,
                       bool *hvilFlag, bool *tempFlag, bool* currentFlag, bool *voltageFlag, bool *socFlag,
                       bool *setMeasure, Elegoo_TFTLCD *tftPtr) {
    /****************
    * Function name:            screenMeasurement
    * Function inputs:          measure variables, their old iteration value, and their respective update flags
    * Function outputs:         Update display with current variables and reset flags
    * Function description:     Controlls the measurement screen with cycled variables and update flags
    * Author(s): 
    *****************/
    Elegoo_TFTLCD tft = *tftPtr;
    unsigned long start = micros();
    if ((*setMeasure) == TRUE) {

        // set title, fonts, data names, prev, and next formatting
        tft.fillScreen(BLACK);
        tft.setTextSize(3);
        tft.setTextColor(WHITE);
        tft.setCursor(15,10); 
        tft.println("Measurements"); 

        tft.setCursor(10,80);
        tft.setTextSize(2);
        tft.println("SOC:");

        tft.setCursor(10,110);
        tft.println("Temperature:");

        tft.setCursor(10,140);
        tft.println("Hv Current:");

        tft.setCursor(10,170);
        tft.println("Hv Voltage:");

        tft.setCursor(10,200);
        tft.println("HVIL:");

        *setMeasure = FALSE;

        *oldTemp = *temperature;
        *oldCurr = *hvCurrent;
        *oldVolt = *hvVoltage;
        *oldSoc  = *stateCharge;
        *oldHVIL = *hvilStatus;

        makeButtons(tftPtr);

        *hvilFlag = TRUE;
        *tempFlag = TRUE;
        *voltageFlag = TRUE;
        *currentFlag = TRUE;
        *socFlag = TRUE;
    }
    if (*hvilFlag) {
      
        // updates print statements for hvilStatus
        tft.setCursor(160, 200);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        if (*oldHVIL == 0) {
            tft.println("CLOSED");
        } else {
            tft.println("OPEN");
        }
        
        tft.setCursor(160, 200);
        tft.setTextSize(2);
        tft.setTextColor(WHITE);
        if (*hvilStatus == 0) {
            tft.println("CLOSED");
        } else {
            tft.println("OPEN");
        }
        *oldHVIL = *hvilStatus;
        *hvilFlag = FALSE;
    }

    if (*tempFlag) {

        // updates print statements for temperature
        updateIntValue(160, 110, BLACK, WHITE, 2, oldTemp, temperature, tempFlag, tftPtr);
    }
    if (*currentFlag) {
        
        // updates print statements for hvCurrent
        updateIntValue(160, 140, BLACK, WHITE, 2, oldCurr, hvCurrent, currentFlag, tftPtr);
    }
    if (*voltageFlag) {
        
        // updates print statements for hvVoltage
        updateIntValue(160, 170, BLACK, WHITE, 2, oldVolt, hvVoltage, voltageFlag, tftPtr);
    }
    if (*socFlag) {
        
        // updates print statements for state of charge
        updateIntValue(160, 80, BLACK, WHITE, 2, oldSoc, stateCharge, socFlag, tftPtr);
    }
}

void screenAlarm(float *hvilAlarm, float *currentAlarm, float *voltageAlarm, bool *currentAlarmFlag,
                 bool *voltageAlarmFlag, bool *hvilAlarmFlag, bool *setAlarm, bool* acknowledgeFlagPtr,
                 char **oldHVILAlarm, char **oldCurrAlarm, char **oldVoltAlarm,
                 char **presentHVILAlarm, char **presentCurrAlarm, char **presentVoltAlarm,
                 bool* acknowledgeButtonFlagPtr, bool* acknowledgeButtonExistsFlagPtr, Elegoo_TFTLCD *tftPtr) {

    /****************
    * Function name:            screenAlarm
    * Function inputs:          Pointer to three alarm variables and their update flags
    * Function outputs:         Updated alarm states for three alarm variables
    * Function description:     Shifts alarm states and updates value on display and active unacknowledge alarms
    * Author(s): 
    *****************/

    Elegoo_TFTLCD tft = *tftPtr;
    unsigned long start = micros();
    if (*setAlarm) {
        // set title, fonts, data names, and button formatting
        tft.fillScreen(BLACK);
        tft.setTextSize(3);
        tft.setTextColor(WHITE);
        //tft.setRotation(2);
        tft.setCursor(70,10); 
        tft.println("Alarms"); 

        tft.setCursor(10,50);
        tft.setTextSize(2);
        tft.println("HVIL:");

        tft.setCursor(10,100);
        tft.println("OC:");

        tft.setCursor(10,150);
        tft.println("Hv Range:");

        *setAlarm = FALSE;
        *presentHVILAlarm = convertAlarm(*hvilAlarm);
        *presentCurrAlarm = convertAlarm(*currentAlarm);
        *presentVoltAlarm = convertAlarm(*voltageAlarm);
        *oldHVILAlarm = convertAlarm(*hvilAlarm);
        *oldCurrAlarm = convertAlarm(*currentAlarm);
        *oldVoltAlarm = convertAlarm(*voltageAlarm);

        makeButtons(tftPtr);

        *hvilAlarmFlag = TRUE;
        *currentAlarmFlag = TRUE;
        *voltageAlarmFlag = TRUE;
    }

    // if any alarm is in state 2, then there is an unacknowledged alarm
    if ((*hvilAlarm == INT_STATE2) || (*currentAlarm == INT_STATE2) || (*voltageAlarm == INT_STATE2)) {
        *acknowledgeFlagPtr = TRUE;
    } else if ((*hvilAlarm != INT_STATE2) && (*currentAlarm != INT_STATE2) && (*voltageAlarm != INT_STATE2)){
        clearAcknowledgeButton(tftPtr);
        *acknowledgeButtonExistsFlagPtr = FALSE;
        *acknowledgeButtonFlagPtr = FALSE;
        *acknowledgeFlagPtr = FALSE;
    }

    // first thing that should interact once the button is pressed in TI
    if ((*acknowledgeFlagPtr == TRUE) && (*acknowledgeButtonExistsFlagPtr == FALSE) && (*acknowledgeButtonFlagPtr == FALSE)) {             // if alarm is active with no button, make button
        acknowledgeButton(tftPtr);
        *acknowledgeButtonExistsFlagPtr = TRUE;
    } else if ((*acknowledgeFlagPtr == TRUE) && (*acknowledgeButtonExistsFlagPtr == TRUE) && (*acknowledgeButtonFlagPtr == TRUE)) {     // if alarm is acknowledged, turn off button
        
        clearAcknowledgeButton(tftPtr);
        *acknowledgeButtonExistsFlagPtr = FALSE;
        *acknowledgeButtonFlagPtr = FALSE;

        // if in state two when alarm is acknowledged, move to state three and override state 2
        if (*hvilAlarm == INT_STATE2) {
            *oldHVILAlarm = convertAlarm(*hvilAlarm);
            *hvilAlarm = INT_STATE3;
            *acknowledgeFlagPtr = FALSE;
            *hvilAlarmFlag = TRUE;
//            Serial.println("I WORK");
        }
        if(*currentAlarm == INT_STATE2) {

            *oldCurrAlarm = convertAlarm(*currentAlarm);
            *currentAlarm = INT_STATE3;
            *currentAlarmFlag = TRUE;
            *acknowledgeFlagPtr = FALSE;
        }
        if (*voltageAlarm == INT_STATE2) {
            *oldVoltAlarm = convertAlarm(*voltageAlarm);
            *voltageAlarm = INT_STATE3;
            *voltageAlarmFlag = TRUE;
            *acknowledgeFlagPtr = FALSE;
        }
    }

    if (*hvilAlarmFlag) {
        
        // updates print statements for next state
        *presentHVILAlarm = convertAlarm(*hvilAlarm);
        updateCharValue(75, 50, BLACK, MAGENTA, 2, oldHVILAlarm, presentHVILAlarm, hvilAlarmFlag, tftPtr);
    }
    if (*currentAlarmFlag) {
//            Serial.println(*currentAlarm);

        // updates print statements for next state
        *presentCurrAlarm = convertAlarm(*currentAlarm);
        updateCharValue(50, 100, BLACK, MAGENTA, 2, oldCurrAlarm, presentCurrAlarm, currentAlarmFlag, tftPtr);
    }
    if (*voltageAlarmFlag) {
        
        // updates print statements for next state
        *presentVoltAlarm = convertAlarm(*voltageAlarm);
        updateCharValue(125, 150, BLACK, MAGENTA, 2, oldVoltAlarm, presentVoltAlarm, voltageAlarmFlag, tftPtr);
    }
    return;
    
}

void screenBattery(bool *contactState, bool *toggleFlag, bool* historicalToggle, 
                   bool *setBattery, bool *oldContact, Elegoo_TFTLCD *tftPtr) {
    /****************
    * Function name:            screenBattery
    * Function inputs:          Pointer to contactor state, toggle flags, battery states and LCD
    * Function outputs:         Updated contactor state
    * Function description:     toggles contactor state if need be
    * Author(s): 
    *****************/
    
    Elegoo_TFTLCD tft = *tftPtr;
    unsigned long start = micros();
    if (*setBattery) {
        
        // set title, fonts, data names, prev, and next formatting
        tft.fillScreen(BLACK);
        tft.setTextSize(3);
        tft.setTextColor(WHITE);
        
        //tft.setRotation();
        tft.setCursor(60,10); 
        tft.println("Battery");
        tft.setCursor(70,40);
        tft.println("ON/OFF"); 

        tft.setCursor(10,100);
        tft.setTextSize(2);
        tft.println("Battery Status:");
        
        //toggle button
        tft.drawRect(70, 180, 100, 50, CYAN);
        tft.fillRect(70, 180, 100, 50, CYAN);
        tft.setCursor(85,200);
        tft.setTextColor(BLACK); 
        tft.setTextSize(2);
        tft.print("Toggle");
        
        *setBattery = FALSE;
        *oldContact = *contactState;

        makeButtons(tftPtr);

        tft.setCursor(200, 100);
        tft.setTextSize(2);
        if (*contactState == CLOSE) {
            tft.setTextColor(GREEN);
            tft.println("ON");
        } else {
            tft.setTextColor(RED);
            tft.println("OFF");
        }
    }
    if (*toggleFlag || *historicalToggle) {

        // flip contact state and turns off flag
        tft.setCursor(200, 100);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);

        // overrides to erase prevous value
        if (*oldContact == CLOSE) {
            tft.println("ON");
        } else {
            tft.println("OFF");
        }

        // prints new value
        tft.setCursor(200, 100);
        tft.setTextSize(2);

        // sets battery state to be ON if contactor is closed
        if (*contactState == CLOSE) {
            tft.setTextColor(GREEN);
            tft.println("ON");
        } else {
            tft.setTextColor(RED);
            tft.println("OFF");
        }
        *oldContact = *contactState;
        *toggleFlag = FALSE;
        *historicalToggle = FALSE;
    }
    return;
}

void screenAccel(float *deltaX, float *deltaY, float *deltaZ, float *distanceTraveled,
                 float *degreeX, float *degreeY, float *degreeZ, bool *setAccel,
                 float *oldDeltaX, float *oldDeltaY, float *oldDeltaZ, float *oldDistanceTraveled,
                 float *oldDegreeX, float *oldDegreeY, float *oldDegreeZ,
                 bool *deltaXFlag, bool *deltaYFlag, bool *deltaZFlag, bool *distanceTraveledFlag,
                 bool *degreeXFlag, bool *degreeYFlag, bool *degreeZFlag, Elegoo_TFTLCD *tftPtr) {
    /****************
    * Function name:            screenAccel
    * Function inputs:          Pointer to accelerometer variables and update flags
    * Function outputs:         Updated display for accelerometer variables
    * Function description:     displays accelerometer variables on Elegoo LCD
    * Author(s): 
    *****************/
    
    Elegoo_TFTLCD tft = *tftPtr;
    unsigned long start = micros();
    if ((*setAccel) == TRUE) {

        // set title, fonts, data names, prev, and next formatting
        tft.fillScreen(BLACK);
        tft.setTextSize(3);
        tft.setTextColor(WHITE);
        tft.setCursor(5,10); 
        tft.println("Accelerometer") ;

        tft.setCursor(10,50);
        tft.setTextSize(2);
        tft.println("Relative Position:");

        tft.setCursor(10,115);
        tft.println("Total Distance:");

        tft.setCursor(10,180);
        tft.println("Static Angle:");

        tft.setCursor(20, 80);
        tft.println("X:");
 
        tft.setCursor(90, 80);
        tft.println("Y:");

        tft.setCursor(160, 80);
        tft.println("Z:");

        tft.setCursor(20, 210);
        tft.println("X:");
        
        tft.setCursor(95, 210);
        tft.println("Y:");

        tft.setCursor(170, 210);
        tft.println("Z:"); 

        *setAccel = FALSE;
        *oldDeltaX = *deltaX;
        *oldDeltaY = *deltaY;
        *oldDeltaZ = *deltaZ;
        *oldDistanceTraveled = *distanceTraveled;
        *oldDegreeX = *degreeX;
        *oldDegreeY = *degreeY;
        *oldDegreeZ = *degreeZ;

        makeButtons(tftPtr);

        *deltaXFlag = TRUE;
        *deltaYFlag = TRUE;
        *deltaZFlag = TRUE;
        *distanceTraveledFlag = TRUE;
        *degreeXFlag = TRUE;
        *degreeYFlag = TRUE;
        *degreeZFlag = TRUE;

        *setAccel = FALSE;
    }
    if (*deltaXFlag) {
        
        // updates print statements for delta X
        updateIntValue(45, 80, BLACK, WHITE, 2, oldDeltaX, deltaX, deltaXFlag, tftPtr);
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(20, 80);
        tft.println("X:");
    }
    if (*deltaYFlag) {
        
        // updates print statements for delta Y
        updateIntValue(115, 80, BLACK, WHITE, 2, oldDeltaY, deltaY, deltaYFlag, tftPtr);
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(90, 80);
        tft.println("Y:");
    }
    if (*deltaZFlag) {
        
        // updates print statements for delta Z
        updateIntValue(185, 80, BLACK, WHITE, 2, oldDeltaZ, deltaZ, deltaZFlag, tftPtr);
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(160, 80);
        tft.println("Z:");
    }
    if (*distanceTraveledFlag) {
        
        // updates print statements for total distance traveled
        updateIntValue(45, 145, BLACK, WHITE, 2, oldDistanceTraveled, distanceTraveled, distanceTraveledFlag, tftPtr);

    }
    if (*degreeXFlag) {
        
        // updates print statements for degree X
        updateIntValue(45, 210, BLACK, WHITE, 2, oldDegreeX, degreeX, degreeXFlag, tftPtr);
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(20, 210);
        tft.println("X:");
    }
    if (*degreeYFlag) {
        
        // updates print statements for degree Y
        updateIntValue(120, 210, BLACK, WHITE, 2, oldDegreeY, degreeY, degreeYFlag, tftPtr);
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(95, 210);
        tft.println("Y:");
    }
    if (*degreeZFlag) {
        
        // updates print statements for hvCurrent
        updateIntValue(195, 210, BLACK, WHITE, 2, oldDegreeZ, degreeZ, degreeZFlag, tftPtr);
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(170, 210);
        tft.println("Z:");
    }
    return;
}
void displayTask(void* mData) {
    /****************
    * Function name:            displayTask
    * Function inputs:          variables to be displayed in measurements, alarms, and battery
    * Function outputs:         updates and sets display for three screens
    * Function description:     calls functions to updates LCD display
    *                           depending on variable flags
    * Author(s): 
    *****************/

    displayData* data = (displayData*) mData;
    Serial.print("I am in accel screen"); Serial.println(*(data->screen_id));
    // if on a non-alarm screen and there is an active not acknowledged alarm, navigate to that screen
    if (((*(data->hvilAlarm) == INT_STATE2) || (*(data->currentAlarm) == INT_STATE2) || 
        (*(data->voltageAlarm) == INT_STATE2)) && (*(data->screen_id) != ALARMSCREEN)) {
        *(data->screen_id) = ALARMSCREEN;
        *(data->setAlarm) = TRUE;
    }
//    } else if ((*(data->screen_id) != ALARMSCREEN)) {
//        *(data->acknowledgeButtonExistsFlag) = FALSE;
//        *(data->acknowledgeButtonFlag) = FALSE;
//    }
// remove this later
//    *(data->screen_id) = 4;
    if (*(data->screen_id) == MEASURESCREEN) {
        screenMeasurement(data->hvilStatus, data->temperature, data->hvCurrent, data->hvVoltage, data->stateCharge,
                          data->oldTemp, data->oldCurr, data->oldVolt, data->oldSoc, data->oldHVIL,
                          data->hvilFlag, data->tempFlag, data->currentFlag, data->voltageFlag, data->socFlag,
                          data->setMeasure, data->tft);
    } else if (*(data->screen_id) == ALARMSCREEN) {
        screenAlarm(data->hvilAlarm, data->currentAlarm, data->voltageAlarm, data->currentAlarmFlag, 
                    data->voltageAlarmFlag, data->hvilAlarmFlag, data->setAlarm, data->acknowledgeFlag,
                    data->oldHVILAlarm, data->oldCurrAlarm, data->oldVoltAlarm, data->presentHVILAlarm,
                    data->presentCurrAlarm, data->presentVoltAlarm,data->acknowledgeButtonFlag,
                    data->acknowledgeButtonExistsFlag, data->tft);
    } else if (*(data->screen_id) == BATTERYSCREEN) {
        screenBattery(data->contactState, data->toggleFlag, data->historicalToggleFlag,
                      data->setBattery, data->oldContact, data->tft);
    } else if (*(data->screen_id) == ACCELSCREEN) {
        screenAccel(data->deltaX, data->deltaY, data->deltaZ, data->distanceTraveled,
                    data->degreeX, data->degreeY, data->degreeZ, data->setAccel,
                    data->oldDeltaX, data->oldDeltaY, data->oldDeltaZ, data->oldDistanceTraveled,
                    data->oldDegreeX, data->oldDegreeY, data->oldDegreeZ,
                    data->deltaXFlag, data->deltaYFlag, data->deltaZFlag, data->distanceTraveledFlag,
                    data->degreeXFlag, data->degreeYFlag, data->degreeZFlag, data->tft);
    }

  return;
}
