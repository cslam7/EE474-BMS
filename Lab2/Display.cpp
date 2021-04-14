#include <stdlib.h>
#include <stdbool.h>
#include "Display.h"

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
    tft.setCursor(35,275);
    tft.setTextColor(WHITE); 
    tft.setTextSize(2);
    tft.print("Prev");

    // next button
    tft.drawRect(130, 255, 100, 50, 0xFFFF);
    tft.fillRect(130, 255, 100, 50, 0x001F);
    tft.setCursor(155,275);
    tft.setTextColor(WHITE); 
    tft.setTextSize(2);
    tft.print("Next");
}

float oldTemp, oldCurr, oldVolt, oldSoc, oldHVIL;
void screenMeasurement(bool *hvilStatus, float *temperature, float *hvCurrent, float *hvVoltage, float *stateCharge,
                       bool *hvilFlag, bool *tempFlag, bool* currentFlag, bool *voltageFlag, bool *socFlag,
                       bool *setMeasure, Elegoo_TFTLCD *tftPtr) {
    /****************
    * Function name:            screenMeasurement
    * Function inputs:          measure variables and their respective update flags
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

        oldTemp = *temperature;
        oldCurr = *hvCurrent;
        oldVolt = *hvVoltage;
        oldSoc  = *stateCharge;
        oldHVIL = *hvilStatus;

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
        if (oldHVIL == 0) {
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
        oldHVIL = *hvilStatus;
        *hvilFlag = FALSE;
    }

    if (*tempFlag) {
        
        // updates print statements for temperature
        int temp = int (*temperature);
        tft.setCursor(160, 110);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.println(oldTemp);
        
        tft.setCursor(160, 110);
        tft.setTextSize(2);
        tft.setTextColor(WHITE);
        tft.println(temp);
        oldTemp = temp;
        *tempFlag = FALSE;

    }
    if (*currentFlag) {
        
        // updates print statements for hvCurrent
        int curr = int (*hvCurrent);
        tft.setCursor(160, 140);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.println(oldCurr);
        
        tft.setCursor(160, 140);
        tft.setTextSize(2);
        tft.setTextColor(WHITE);
        tft.println(curr);
        oldCurr = curr;
        *currentFlag = FALSE;
    }
    if (*voltageFlag) {
        
        // updates print statements for hvVoltage
        int volt = int (*hvVoltage);
        tft.setCursor(160, 170);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.println(oldVolt);
        
        tft.setCursor(160, 170);
        tft.setTextSize(2);
        tft.setTextColor(WHITE);
        tft.println(volt);
        oldVolt = volt;
        *voltageFlag = FALSE;
    }
    if (*socFlag) {
        
        // updates print statements for state of charge
        int soc = int (*stateCharge);
        tft.setCursor(160, 80);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.println(oldSoc);
        tft.setCursor(160, 80);
        tft.setTextSize(2);
        tft.setTextColor(WHITE);
        tft.println(soc);
        oldSoc = soc;
    }
}

char *oldHVILAlarm, *oldCurrAlarm, *oldVoltAlarm;
char *presentHVILAlarm, *presentCurrAlarm, *presentVoltAlarm;
void screenAlarm(float *hvilAlarm, float *currentAlarm, float *voltageAlarm,
                 bool *currentAlarmFlag, bool *voltageAlarmFlag, bool *hvilAlarmFlag,
                 bool *setAlarm, Elegoo_TFTLCD *tftPtr) {

    /****************
    * Function name:            screenAlarm
    * Function inputs:          Pointer to three alarm variables and their update flags
    * Function outputs:         Updated alarm states for three alarm variables
    * Function description:     Shifts alarm states and updates value on display
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

        tft.setCursor(10,120);
        tft.println("OC:");

        tft.setCursor(10,185);
        tft.println("Hv Range:");

        *setAlarm = FALSE;
        presentHVILAlarm = convertAlarm(*hvilAlarm);
        presentCurrAlarm = convertAlarm(*currentAlarm);
        presentVoltAlarm = convertAlarm(*voltageAlarm);
        oldHVILAlarm = convertAlarm(*hvilAlarm);
        oldCurrAlarm = convertAlarm(*currentAlarm);
        oldVoltAlarm = convertAlarm(*voltageAlarm);

        makeButtons(tftPtr);

        *hvilAlarmFlag = TRUE;
        *currentAlarmFlag = TRUE;
        *voltageAlarmFlag = TRUE;
    }
    
    if (*hvilAlarmFlag) {
        
        // updates print statements for next state
        presentHVILAlarm = convertAlarm(*hvilAlarm);
        tft.setCursor(20, 75);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.println(oldHVILAlarm);
        
        tft.setCursor(20, 75);
        tft.setTextSize(2);
        tft.setTextColor(MAGENTA);
        tft.println(presentHVILAlarm);
        oldHVILAlarm = presentHVILAlarm;
        *hvilAlarmFlag = FALSE;
    }
    if (*currentAlarmFlag) {
        
        // updates print statements for next state
        presentCurrAlarm = convertAlarm(*currentAlarm);
        tft.setCursor(20, 145);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.println(oldCurrAlarm);
        
        tft.setCursor(20, 145);
        tft.setTextSize(2);
        tft.setTextColor(MAGENTA);
        tft.println(presentCurrAlarm);
        oldCurrAlarm = presentCurrAlarm;
        *currentAlarmFlag = FALSE;
    }
    if (*voltageAlarmFlag) {
        
        // updates print statements for next state
        presentVoltAlarm = convertAlarm(*voltageAlarm);
        tft.setCursor(20, 210);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.println(oldVoltAlarm);

        tft.setCursor(20, 210);
        tft.setTextSize(2);
        tft.setTextColor(MAGENTA);
        tft.println(presentVoltAlarm);
        oldVoltAlarm = presentVoltAlarm;
        *voltageAlarmFlag = FALSE;
    }
    return;
    
}

bool oldContact;
void screenBattery(bool *contactState, bool *toggleFlag, bool *setBattery, Elegoo_TFTLCD *tftPtr) {
    /****************
    * Function name:            updateHvCurrent
    * Function inputs:          Pointer to present current  value, old current update flag
    *                           time sync counter
    * Function outputs:         Updated current value and flag
    * Function description:     Shifts current value every 2s function call and sets
    *                           update flag if true
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
        oldContact = *contactState;

        makeButtons(tftPtr);

        // sets battery state on startup
        tft.setCursor(200, 100);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        if (oldContact == 1) {
            tft.println("ON");
        } else {
            tft.println("OFF");
        }
        
        tft.setCursor(200, 100);
        tft.setTextSize(2);
        if (*contactState == 1) {
            tft.setTextColor(GREEN);
            tft.println("ON");
        } else {
            tft.setTextColor(RED);
            tft.println("OFF");
        }
    }
    if (*toggleFlag) {
        
        // flip contact state and turns off flag
        tft.setCursor(200, 100);
        tft.setTextColor(BLACK);
        tft.setTextSize(2);

        // overrides to erase prevous value
        if (oldContact == 1) {
            tft.println("ON");
        } else {
            tft.println("OFF");
        }

        // prints new value
        tft.setCursor(200, 100);
        tft.setTextSize(2);

        // sets battery state to be ON if contactor is on
        if (*contactState == 1) {
            tft.setTextColor(GREEN);
            tft.println("ON");
        } else {
            tft.setTextColor(RED);
            tft.println("OFF");
        }
        oldContact = *contactState;
//        *contactState = !(*contactState);
        *toggleFlag = FALSE;
    }
    return;
}

void displayTask(void* mData) {
    /****************
    * Function name:       displayTask
    * Function inputs:      all global variables
    * Function outputs:     updates and sets display for three screens
    * Function description:   calls functions to updates LCD display
    *                           depending on variable flags
    * Author(s): 
    *****************/
    displayData* data = (displayData*) mData;

     if (*(data->screen_id) == 1) {
         screenMeasurement(data->hvilStatus, data->temperature, data->hvCurrent, data->hvVoltage, data->stateCharge,
                           data->hvilFlag, data->tempFlag, data->currentFlag, data->voltageFlag, data->socFlag,
                           data->setMeasure, data->tft);
     } else if (*(data->screen_id) == 2) {
        screenAlarm(data->hvilAlarm, data->currentAlarm, data->voltageAlarm,
                    data->currentAlarmFlag, data->voltageAlarmFlag, data->hvilAlarmFlag,
                    data->setAlarm, data->tft);
     } else if (*(data->screen_id) == 3) {
         screenBattery(data->contactState, data->toggleFlag, data->setBattery, data->tft);
     }
  return;
}
