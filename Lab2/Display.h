
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

#define STATE1 "NOT ACTIVE"
#define STATE2 "ACTIVE, NOT ACKNOWLEDGED"
#define STATE3 "ACTIVE, ACKNOWLEDGED"

extern Elegoo_TFTLCD tft;

typedef struct displayTaskData {
    bool* hvilStatus;               // status of hvil from test circuit
    float* temperature;             // present value of temp
  	float* hvCurrent;               // present value of current
	float* hvVoltage;               // present value of voltage
    float* stateCharge;             // present value of SOC
    bool *hvilFlag, *tempFlag, *currentFlag, *voltageFlag, *socFlag;       // flags to update fields in display
    
    float *hvilAlarm, *currentAlarm, *voltageAlarm;             // pointer to current state of each alarm
    bool *currentAlarmFlag, *voltageAlarmFlag, *hvilAlarmFlag;  // pointer signaling soc has updated 
    
    bool* contactState;             // pointer to current state of each alarm
    bool* toggleFlag;               // pointer signaling soc has updated 

    bool *setMeasure;               // condition to check if setup for measure screen should occur
    bool *setAlarm;                 // condition to check if setup for alarm screen should occur
    bool *setBattery;               // variable to initialize screen formatting

    bool *toggle;                   // inputs from touch input

    int *screen_id;                 // screen number
    
    Elegoo_TFTLCD *tft;             // LCD display
} displayData;


void displayTask (void* mData);

void screenMeasurement(bool *hvilStatus, float *temperature, float *hvCurrent, float *hvVoltage, float *stateCharge,
                       bool *hvilFlag, bool *tempFlag, bool* currentFlag, bool *voltageFlag, bool *socFlag,
                       bool *setMeasure, Elegoo_TFTLCD *tftPtr);

void screenAlarm(float *hvilAlarm, float *currentAlarm, float *voltageAlarm,
                 bool *currentAlarmFlag, bool *voltageAlarmFlag, bool *hvilAlarmFlag,
                 bool *setAlarm, Elegoo_TFTLCD *tftPtr);
                 
void screenBattery(bool *contactState, bool *toggleFlag, bool *setBattery, Elegoo_TFTLCD *tftPtr);



void makeButtons(Elegoo_TFTLCD *tftPtr);

char* convertAlarm(float state);

#endif
 
