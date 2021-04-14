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
        int temp = int (*temperature);
        tft.setCursor(160, 110);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.println(*oldTemp);
        
        tft.setCursor(160, 110);
        tft.setTextSize(2);
        tft.setTextColor(WHITE);
        tft.println(temp);
        *oldTemp = temp;
        *tempFlag = FALSE;

    }
    if (*currentFlag) {
        
        // updates print statements for hvCurrent
        int curr = int (*hvCurrent);
        tft.setCursor(160, 140);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.println(*oldCurr);
        
        tft.setCursor(160, 140);
        tft.setTextSize(2);
        tft.setTextColor(WHITE);
        tft.println(curr);
        *oldCurr = curr;
        *currentFlag = FALSE;
    }
    if (*voltageFlag) {
        
        // updates print statements for hvVoltage
        int volt = int (*hvVoltage);
        tft.setCursor(160, 170);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.println(*oldVolt);
        
        tft.setCursor(160, 170);
        tft.setTextSize(2);
        tft.setTextColor(WHITE);
        tft.println(volt);
        *oldVolt = volt;
        *voltageFlag = FALSE;
    }
    if (*socFlag) {
        
        // updates print statements for state of charge
        int soc = int (*stateCharge);
        tft.setCursor(160, 80);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.println(*oldSoc);
        tft.setCursor(160, 80);
        tft.setTextSize(2);
        tft.setTextColor(WHITE);
        tft.println(soc);
        *oldSoc = soc;
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

//    Serial.print("Button Exists?          "); Serial.println(*acknowledgeButtonExistsFlagPtr);
//    Serial.print("Button Pressed?         "); Serial.println(*acknowledgeButtonFlagPtr);
//    Serial.print("Is there state2 alarm?  "); Serial.println(*acknowledgeFlagPtr);
//    Serial.print("Does this enter the update hvil    "); Serial.println(*hvilAlarmFlag);
    if (*hvilAlarmFlag) {
        
        // updates print statements for next state
        *presentHVILAlarm = convertAlarm(*hvilAlarm);
        tft.setCursor(75, 50);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.println(*oldHVILAlarm);
//        Serial.println(*oldHVILAlarm);
//        Serial.println(*presentHVILAlarm);
        tft.setCursor(75, 50);
        tft.setTextSize(2);
        tft.setTextColor(MAGENTA);
        tft.println(*presentHVILAlarm);
        *oldHVILAlarm = *presentHVILAlarm;
        *hvilAlarmFlag = FALSE;
    }
    if (*currentAlarmFlag) {
//            Serial.println(*currentAlarm);

        // updates print statements for next state
        *presentCurrAlarm = convertAlarm(*currentAlarm);
        tft.setCursor(50, 100);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.println(*oldCurrAlarm);
        
        tft.setCursor(50, 100);
        tft.setTextSize(2);
        tft.setTextColor(MAGENTA);
        tft.println(*presentCurrAlarm);
        *oldCurrAlarm = *presentCurrAlarm;
        *currentAlarmFlag = FALSE;
    }
    if (*voltageAlarmFlag) {
        
        // updates print statements for next state
        *presentVoltAlarm = convertAlarm(*voltageAlarm);
        tft.setCursor(125, 150);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.println(*oldVoltAlarm);

        tft.setCursor(125, 150);
        tft.setTextSize(2);
        tft.setTextColor(MAGENTA);
        tft.println(*presentVoltAlarm);
        *oldVoltAlarm = *presentVoltAlarm;
        *voltageAlarmFlag = FALSE;
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
    }

     // turns off toggle
//    (*(data->toggleFlag)) = FALSE;
  return;
}
