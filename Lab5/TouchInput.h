#ifndef TOUCHINPUT_H_
#define TOUCHINPUT_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include "TouchScreen.h"

//extern TouchScreen ts;
//extern Elegoo_TFTLCD tft;

#define TRUE 1
#define FALSE 0

#define OFF 0

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//#define TS_MINX 150
//#define TS_MINY 120
//#define TS_MAXX 920
//#define TS_MAXY 940

#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define MEASURESCREEN 1
#define ALARMSCREEN 2
#define BATTERYSCREEN 3
#define ACCELSCREEN 4

typedef struct touchInputTaskData {
    int *screenNum;                                      // screen number to rotate through for prev and next and to actuate toggle
    volatile bool *toggleFlag;                           // flag for actuating toggle
    bool *historicalToggleFlag;                          // flag for historical toggle value in non-display cycles
    bool *setMeasure, *setAlarm, *setBattery, *setAccel; // condition to set up formatting for respective screens
    TouchScreen *ts;                                     // touchscreen aspect of Elegoo LCD
    volatile bool *acknowledgeFlag;                      // flag if there is an active but not acknowledged alarm
    volatile bool *acknowledgeButtonFlag;                // flag for if acknowledge button is actuated
    Elegoo_TFTLCD *tft;                                  // display aspect of Elegoo LCD
} touchInputData;

void touchInputTask(void* mData);

void getInput(int *screenNum, bool *toggleFlag, bool *historicalToggleFlag, bool *setMeasure, bool *setAlarm, bool *setBattery,
              bool *setAccel, TouchScreen *tsPtr, bool *acknowledgeFlagPtr, bool *acknowledgeButtonFlagPtr, Elegoo_TFTLCD *tftPtr);
              
#endif
