
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
    bool *setBattery;               // variable to initialize screen formatting
    int *screen_id;                 // screen number

    volatile bool* acknowledgeFlag;              // whether there is an active but not acknowledged alarm
    bool* acknowledgeButtonFlag;                 // whether acknowledge button has been pressed
    volatile bool* acknowledgeButtonExistsFlag;  // checks to see if acknowledge button should be displayed

    char **oldHVILAlarm, **oldCurrAlarm, **oldVoltAlarm;                // old alarm states
    char **presentHVILAlarm, **presentCurrAlarm, **presentVoltAlarm;    // present alarm states
    
    Elegoo_TFTLCD *tft;             // LCD display
} displayData;

void displayTask (void* mData);

void screenMeasurement(bool *hvilStatus, float *temperature, float *hvCurrent, float *hvVoltage, float *stateCharge,
                       float *oldtemp, float *oldCurr, float *oldVolt, float *oldSoc, float *oldHVIL,
                       bool *hvilFlag, bool *tempFlag, bool* currentFlag, bool *voltageFlag, bool *socFlag,
                       bool *setMeasure, Elegoo_TFTLCD *tftPtr);

void screenAlarm(float *hvilAlarm, float *currentAlarm, float *voltageAlarm, bool *currentAlarmFlag,
                 bool *voltageAlarmFlag, bool *hvilAlarmFlag, bool *setAlarm, bool* acknowledgeFlagPtr,
                 char **oldHVILAlarm, char **oldCurrAlarm, char **oldVoltAlarm,
                 char **presentHVILAlarm, char **presentCurrAlarm, char **presentVoltAlarm,
                 bool* acknowledgeButtonFlagPtr, bool* acknowledgeButtonExistsFlagPtr, Elegoo_TFTLCD *tftPtr);
                 
void screenBattery(bool *contactState, bool *toggleFlag, bool* historicalToggle, 
                   bool *setBattery, bool *oldContact, Elegoo_TFTLCD *tftPtr);

void makeButtons(Elegoo_TFTLCD *tftPtr);

char* convertAlarm(float state);

void acknowledgeButton(Elegoo_TFTLCD *tftPtr);

void clearAcknowledgeButton(Elegoo_TFTLCD *tftPtr);

#endif
 
