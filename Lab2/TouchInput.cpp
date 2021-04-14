#include <stdlib.h>
#include <stdbool.h>

#include "TouchInput.h"
#include "Display.h"


#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

void getInput(int *screenNum, bool *toggleFlag, bool *setMeasure, bool *setAlarm, bool *setBattery, TouchScreen *tsPtr, Elegoo_TFTLCD *tftPtr) {

    Elegoo_TFTLCD tft = *tftPtr;
    TouchScreen ts = *tsPtr;
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

//    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {

    // scale from 0->1023 to tft.width
//        p.x = map(p.x, TS_MINX, TS_MAXX, 0, 320);
//        p.y = map(p.y, TS_MINY, TS_MAXY, 0, 240);
        // sets coordinate axis for touch input
        p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
        p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, 0, 320));

        // This is to check for the  for the previous button 

        // if next button is pressed, else if next is pressed
        if ((p.x >= 10) && (p.x<= 110) && (p.y >= 255)  && (p.y <= 305)) {
            // change previous flag
            if (*screenNum == MEASURESCREEN) {
                *screenNum = BATTERYSCREEN;
                *setBattery = TRUE;
            } else if (*screenNum == ALARMSCREEN) {
                *screenNum = MEASURESCREEN;
                *setMeasure = TRUE;
            } else {
              *screenNum = ALARMSCREEN;
              *setAlarm = TRUE;
            }
        } else if ((p.x >= 130) && (p.x<= 230) && (p.y >= 255)  && (p.y <= 305)) {
            //change next flag
            if (*screenNum == MEASURESCREEN) {
                *screenNum = ALARMSCREEN;
                *setAlarm = TRUE;
            } else if (*screenNum == ALARMSCREEN) {
                *screenNum = BATTERYSCREEN;
                *setBattery = TRUE;;
            } else {
              *screenNum = MEASURESCREEN;
              *setMeasure = TRUE;
            }
//            tft.fillScreen(BLACK);
            
        }

        // if toggle is pressed while on battery screen, then update flag
        if (*screenNum == BATTERYSCREEN) {
            if ((p.x >= 70) && (p.x<= 170) && (p.y >= 180) && (p.y <= 230)) {
                // toggle flag
                *toggleFlag = TRUE;
            }
        }
//}
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
//    Serial.println("Got input");
  	getInput(data->screenNum, data->toggleFlag, data->setMeasure, data->setAlarm, data->setBattery, data->ts, data->tft);
	return;
}
