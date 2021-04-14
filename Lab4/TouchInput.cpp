#include <stdlib.h>
#include <stdbool.h>

#include "TouchInput.h"
#include "Display.h"


#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

void getInput(int *screenNum, bool *toggleFlag, bool *historicalToggleFlag, bool *setMeasure, bool *setAlarm, bool *setBattery,
              TouchScreen *tsPtr, bool *acknowledgeFlagPtr, bool *acknowledgeButtonFlagPtr, Elegoo_TFTLCD *tftPtr) {
    /****************
    * Function name: 			getInput
    * Function inputs: 			screen numbers, toggle flags, updates for each display screens,
    *                           touchscreen and LCD variables
    * Function outputs: 		N/A
    * Function description: 	calls functions to updates LCD display and navigate between screens
    *                           depending on variable flags
    * Author(s): 
    *****************/



    Elegoo_TFTLCD tft = *tftPtr;
    TouchScreen ts = *tsPtr;
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);


    // sets coordinate axis for touch input
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
    p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, 0, 320));

    // if next button is pressed, else if next is pressed
    if (*acknowledgeFlagPtr == FALSE) {
        if ((p.x >= 10) && (p.x<= 110) && (p.y >= 255)  && (p.y <= 305)) {
            // change previous flag
            if (*screenNum == MEASURESCREEN) {
                *screenNum = BATTERYSCREEN;
                *setBattery = TRUE;
//                Serial.println("prev to battery");
            } else if (*screenNum == ALARMSCREEN) {
                *screenNum = MEASURESCREEN;
                *setMeasure = TRUE;
//                Serial.println("prev to measure");
            } else {
              *screenNum = ALARMSCREEN;
              *setAlarm = TRUE;
//              Serial.println("prev to alarm");
            }
        } else if ((p.x >= 130) && (p.x<= 230) && (p.y >= 255)  && (p.y <= 305)) {
            //change next flag
            if (*screenNum == MEASURESCREEN) {
                *screenNum = ALARMSCREEN;
                *setAlarm = TRUE;
//                Serial.println("next to alarm");
            } else if (*screenNum == ALARMSCREEN) {
                *screenNum = BATTERYSCREEN;
                *setBattery = TRUE;;
//                Serial.println("next to battery");
            } else {
                *screenNum = MEASURESCREEN;
                *setMeasure = TRUE;
//                Serial.println("next to measure");
            }            
        }
    }

    // if toggle is pressed while on battery screen, then update flag
    if (*screenNum == BATTERYSCREEN) {
        if ((p.x >= 70) && (p.x<= 170) && (p.y >= 180) && (p.y <= 230)) {
            // toggle flag
            *toggleFlag = TRUE;
            *historicalToggleFlag = TRUE;
        } else {
            *toggleFlag = FALSE;
        }
    }

    // if acknowledge is pressed while on battery screen and an active alarm, then acknowledge alarm
    if ((*screenNum == ALARMSCREEN) && (*acknowledgeFlagPtr == TRUE)) {
        if ((p.x >= 45) && (p.x<= 195) && (p.y >= 190) && (p.y <= 240)) {
            // acknowledge button flag
            *acknowledgeButtonFlagPtr = TRUE;     // if any alarms at in state2, go to state3
        }
    }
}

void touchInputTask(void* mData) {
    /****************
    * Function name: 			displayTask
    * Function inputs: 			all global variables
    * Function outputs: 		updates and sets display for three screens
    * Function description: 	calls functions to updates LCD display
    *                           depending on variable flags
    * Author(s): 
    *****************/
  	touchInputData* data = (touchInputData*) mData;
  	getInput(data->screenNum, data->toggleFlag, data->historicalToggleFlag, data->setMeasure, data->setAlarm,
  	         data->setBattery, data->ts, data->acknowledgeFlag, data->acknowledgeButtonFlag, data->tft);
	return;
}
