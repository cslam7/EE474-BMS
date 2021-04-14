
#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define TRUE 1
#define FALSE 0

#define INT_STATE1 0
#define INT_STATE2 1
#define INT_STATE3 2

#define STATE1 "NOT ACTIVE"
#define STATE2 "ACTIVE, NOT ACKNOWLEDGED"
#define STATE3 "ACTIVE, ACKNOWLEDGED"

#define MEASURESCREEN 1
#define ALARMSCREEN 2
#define BATTERYSCREEN 3
#define ACCELSCREEN 4

#define OPEN 1
#define CLOSE 0

extern Elegoo_TFTLCD tft;

typedef struct displayTaskData {
    bool* hvilStatus;               // status of hvil from test circuit
    float* temperature;             // present value of temp
    float* hvCurrent;               // present value of current
    float* hvVoltage;               // present value of voltage
    float* stateCharge;             // present value of SOC
    float *oldTemp, *oldCurr, *oldVolt, *oldSoc, *oldHVIL;                 // old values of measurement variables
    bool *hvilFlag, *tempFlag, *currentFlag, *voltageFlag, *socFlag;       // flags to update fields in display
    
    volatile float *hvilAlarm;                   // pointer to hvil alarm state
    float *currentAlarm, *voltageAlarm;          // pointer to current state of current and voltage
    bool *currentAlarmFlag, *voltageAlarmFlag;   // pointer to update flags for current and voltage
    volatile bool *hvilAlarmFlag;                // pointer signaling alarms have updated 
    
    volatile bool* contactState;             // pointer to current state of contactor
    volatile bool *oldContact;               // pointer to old state of contactor
    volatile bool* toggleFlag;               // pointer signaling contactor has updated
    bool* historicalToggleFlag;              // pointer to preserved toggle flag in non-display cycles

    bool *setMeasure;               // condition to check if setup for measure screen should occur
    bool *setAlarm;                 // condition to check if setup for alarm screen should occur
    bool *setBattery;               // variable to initialize screen formatting on battery screen
    bool *setAccel;                 // variable to initialize screen formatting on accelerometer screen
    int *screen_id;                 // screen number

    volatile bool* acknowledgeFlag;              // whether there is an active but not acknowledged alarm
    bool* acknowledgeButtonFlag;                 // whether acknowledge button has been pressed
    volatile bool* acknowledgeButtonExistsFlag;  // checks to see if acknowledge button should be displayed

    char **oldHVILAlarm, **oldCurrAlarm, **oldVoltAlarm;                // old alarm states
    char **presentHVILAlarm, **presentCurrAlarm, **presentVoltAlarm;    // present alarm states

    float *deltaX;                   // relative position from origion via x axis
    float *deltaY;                   // relative position from origion via y axis
    float *deltaZ;                   // relative position from origion via z axis
    float *distanceTraveled;         // total distance traveled from startup
    float *degreeX;                  // relative rotation from origion via x axis
    float *degreeY;                  // relative rotation from origion via y axis
    float *degreeZ;                  // relative rotation from origion via z axis

    float *oldDeltaX;                // old relative position from origion via x axis
    float *oldDeltaY;                // old relative position from origion via y axis
    float *oldDeltaZ;                // old relative position from origion via z axis
    float *oldDistanceTraveled;      // old distance trveled
    float *oldDegreeX;               // old relative rotation from origion via x axis
    float *oldDegreeY;               // old relative rotation from origion via y axis
    float *oldDegreeZ;               // old relative rotation from origion via z axis

    bool *deltaXFlag;                // update deltaX on display
    bool *deltaYFlag;                // update deltaY on display
    bool *deltaZFlag;                // update deltaZ on display
    bool *distanceTraveledFlag;      // update distance traveled on display
    bool *degreeXFlag;               // update degreeX on display
    bool *degreeYFlag;               // update degreeY on display
    bool *degreeZFlag;               // update degreeX on display


    Elegoo_TFTLCD *tft;             // LCD display
} displayData;

void displayTask(void* mData);

void screenMeasurement(bool *hvilStatus, float *temperature, float *hvCurrent, float *hvVoltage, float *stateCharge,
                       float *oldtemp, float *oldCurr, float *oldVolt, float *oldSoc, float *oldHVIL,
                       bool *hvilFlag, bool *tempFlag, bool* currentFlag, bool *voltageFlag, bool *socFlag,
                       bool *setMeasure, Elegoo_TFTLCD *tftPtr);

void screenAlarm(float *hvilAlarm, float *currentAlarm, float *voltageAlarm, bool *currentAlarmFlag,
                 bool *voltageAlarmFlag, bool *hvilAlarmFlag, bool *setAlarm, bool* acknowledgeFlagPtr,
                 char **oldHVILAlarm, char **oldCurrAlarm, char **oldVoltAlarm,
                 char **presentHVILAlarm, char **presentCurrAlarm, char **presentVoltAlarm,
                 bool* acknowledgeButtonFlagPtr, bool* acknowledgeButtonExistsFlagPtr, Elegoo_TFTLCD *tftPtr);

void screenAccel(float *deltaX, float *deltaY, float *deltaZ, float *distanceTraveled,
                 float *degreeX, float *degreeY, float *degreeZ, bool *setAccel,
                 float *oldDeltaX, float *oldDeltaY, float *oldDeltaZ, float *oldDistanceTraveled,
                 float *oldDegreeX, float *oldDegreeY, float *oldDegreeZ,
                 bool *deltaXFlag, bool *deltaYFlag, bool *deltaZFlag, bool *distanceTraveledFlag,
                 bool *degreeXFlag, bool *degreeYFlag, bool *degreeZFlag, Elegoo_TFTLCD *tftPtr);
                 
void screenBattery(bool *contactState, bool *toggleFlag, bool* historicalToggle, 
                   bool *setBattery, bool *oldContact, Elegoo_TFTLCD *tftPtr);

void updateIntValue(int x, int y, int oldColor, int newColor, int textSize, float *oldValue,
                    float *newValue, bool *updateFlag, Elegoo_TFTLCD *tftPtr);

void updateCharValue(int x, int y, int oldColor, int newColor, int textSize, char **oldValue,
                     char **newValue, bool *updateFlag, Elegoo_TFTLCD *tftPtr);
                                         
void makeButtons(Elegoo_TFTLCD *tftPtr);

char* convertAlarm(float state);

void acknowledgeButton(Elegoo_TFTLCD *tftPtr);

void clearAcknowledgeButton(Elegoo_TFTLCD *tftPtr);

#endif
 
