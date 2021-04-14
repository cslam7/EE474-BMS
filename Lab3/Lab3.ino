#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <TimerOne.h>      // library for timed interrupt support
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library

// may need to add ISR routine
#include "Scheduler.h"
#include "Measurement.h"
#include "TaskControlBlock.h"
#include "StateOfCharge.h"
#include "Contactor.h"
#include "Alarm.h"
#include "Display.h"
#include "TouchInput.h"

#define TRUE 1
#define FALSE 0

#define NUMTASKS 6
#define NUMSTATES 3

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//Task Control Blocks
TCB measurementTCB;         // Declare measurement TCB
TCB displayTCB;         	// Declare display TCB
TCB touchInputTCB;			// Declare touchinput TCB
TCB alarmTCB;               // Declare alarm TCB
TCB contactorTCB;           // Declare contactor TCB
TCB socTCB;                 // Declare state of charge TCB

//TCB* taskQueue[] = {&displayTCB, &touchInputTCB, &measurementTCB, &alarmTCB, &contactorTCB, &socTCB};				// Declare array of TCBS to hold task functions

// Measurement Data
measurementData measure;     // Declare measurement data structure - defined in Measurement.h
displayData display; 		 // Declare display data structure - defined in Display.h
touchInputData touchInput;	 // Declare touchinput data structure - defined in TouchInput.h
alarmData alarm;             // Declare alarm data structure - defined in Alarm.h
contactorData contactor;     // Declare contactor data structure - defined in Contactor.h
socData soc;                 // Declare state of charge data structure - defined in StateofCharge.h

bool taskFlag[NUMTASKS] = {TRUE, TRUE, TRUE,
               			   TRUE, TRUE, TRUE};     // array to determine execution conditions for each task

float hvCurrent = -20;          // present Current set to its initial state
float hvVoltage = 10;           // present Voltage set to its initial state
float temperature = -10;        // present Temp set to its initial state
bool HVIL = FALSE;              // set HVIL output to initial state
const byte hvilPin  = 21;       // input pin via ATMega board
bool hvilFlag = TRUE;           // will update hvil on display
bool tempFlag = TRUE;           // will update temp on display 
const byte tempPin  = A11;      // input pin via ATMega board
bool currentFlag= TRUE;         // will update current on display
const byte currentPin  = A13;   // input pin via ATMega board
bool voltageFlag = TRUE;        // will update voltage on display
const byte voltagePin  = A15;   // input pin via ATMega board

volatile float hvilAlarm = 0;   // set initial state for hvil alarm
float currentAlarm = 0;         // set initial state for current alarm
float voltageAlarm = 0;         // set initial state for voltage alarm

volatile bool hvilAlarmFlag = TRUE;      // will update hvil alarm on display
bool currentAlarmFlag = TRUE;            // will update current alarm on display
bool voltageAlarmFlag = TRUE;            // will update voltage alarm on display

float stateCharge = 0;          // set initial state for the SOC
bool socFlag = TRUE;            // will update SOC on display

volatile bool contactState = TRUE;       // set initial state for the contactor
volatile bool oldContact = TRUE;
volatile bool toggleFlag = FALSE;        // will not toggle contactor state on start up
volatile byte contactPin = 37;           // output pin for contactor
//volatile byte state = LOW;             // current state of contactor

bool setMeasure = TRUE;         // updates formatting for measurement screen on display
bool setAlarm = TRUE;           // updates formatting for alarm scren on display
bool setBattery = TRUE;         // updates formatting on battery on/off screen on display

// int timeCount = 1;           // global time counter that syncs up variable cycling

int screenNum = 1;              // screen number {measurement = 1, alarm = 2, battery on/off = 3}

volatile bool timerFlag = 0;    // declare timerISR flag

bool acknowledgeButtonFlag = FALSE;                      // status of the alarm button
volatile bool acknowledgeButtonExistsFlag = FALSE;       // if the button is displayed on the screen or not
volatile bool acknowledgeFlag = FALSE;                   // if there is any alarm that must be acknowledged

volatile int counter = 0;       // // global time counter for debugging

float oldTemp = -10;
float oldCurr = -20;
float oldVolt = 10;
float oldSoc = 0;
float oldHVIL = FALSE;;

char *oldHVILAlarm;
char *oldCurrAlarm;
char *oldVoltAlarm;
char *presentHVILAlarm;
char *presentCurrAlarm;
char *presentVoltAlarm;

void setup() {  
  /****************
    * Function name: setup
    * Function inputs: None
    * Function outputs: None
    * Function description: sets up the scheduler, global time base, and initiates LCD.
    * Author(s): 
    *****************/

    Serial.begin(9600);                   // Sets serial data transmission rate at 9600 bits/s (baud)
    Serial.println(F("TFT LCD test"));    // prints string to stating test will occur without using ram


    #ifdef USE_Elegoo_SHIELD_PINOUT         // This is defined in Elegoo_TFTLCD.h               
        Serial.println(F("Using Elegoo 2.4\" TFT Arduino Shield Pinout"));      // prints if the TFT is plugged in
    #else
        Serial.println(F("Using Elegoo 2.4\" TFT Breakout Board Pinout"));      // prints if the TFT is not plugged in
                                                                                // means not just dedicated pins are being used
    #endif

    Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());     // prints out dimensions of tft

    tft.reset();                                            // sets LCD display to default screen

     uint16_t identifier = tft.readID();                    // stores LCD ID as a unsigned short to check for respective drivers
     if(identifier == 0x9325) {                             // checks if LCD ID is 0x9325
        Serial.println(F("Found ILI9325 LCD driver"));      // if so, prints out that its respective driver is found
    } else if(identifier == 0x9328) {                       // checks if LCD ID is 0x9328
        Serial.println(F("Found ILI9328 LCD driver"));      // if so, prints out that its respective driver is found
    } else if(identifier == 0x4535) {                       // checks if LCD ID is 0x4535
        Serial.println(F("Found LGDP4535 LCD driver"));     // if so, prints out that its respective driver is found
    }else if(identifier == 0x7575) {                        // checks if LCD ID is 0x7575
        Serial.println(F("Found HX8347G LCD driver"));      // if so, prints out that its respective driver is found
    } else if(identifier == 0x9341) {                       // checks if LCD ID is 0x9341
        Serial.println(F("Found ILI9341 LCD driver"));      // if so, prints out that its respective driver is found
    } else if(identifier == 0x8357) {                       // checks if LCD ID is 0x8357
        Serial.println(F("Found HX8357D LCD driver"));      // if so, prints out that its respective driver is found
    } else if(identifier==0x0101) {                         // checks if LCD ID is 0x0101     
        identifier=0x9341;                                  // if so, change LCD ID variable to equal 0x9341
        Serial.println(F("Found 0x9341 LCD driver"));       // prints out its respective driver is found
    } else if(identifier==0x1111) {                         // checks if LCD ID is 0x1111
        identifier=0x9328;                                  // if so, change LCD ID variable to equal 0x9341
        Serial.println(F("Found 0x9328 LCD driver"));       // prints out its respective driver is found
    } else {                                                // enters statement if LCD ID does not match any of the above
        Serial.print(F("Unknown LCD driver chip: "));                                     // prints out unkonwn ID value and store value in RAM without newline
        Serial.println(identifier, HEX);                                                  // prints out identifier code in HEX
        Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));     // prints out first line of instructions to decipher unknown LCD driver
        Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));                          // prints out possible missing line of code if using Arduino shield
        Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));         // prints out possible location of missing line of code
        Serial.println(F("If using the breakout board, it should NOT be #defined!"));     // prints out possible syntax errors  if using breakout board
        Serial.println(F("Also if using the breakout, double-check that all wiring"));    // prints out statement to check hardware correctness
        Serial.println(F("matches the tutorial."));                                       // prints out end of statement in line above
        identifier=0x9328;
    }
    
  	tft.begin(identifier);        // initiates board with most recent LCD ID

    // Initialize Measurement & Sensors
    measure                     = {&HVIL, &hvilPin, &temperature, &hvCurrent, &hvVoltage,
    							   &hvilFlag, &tempFlag, &currentFlag, &voltageFlag,
								   &tempPin, &currentPin, &voltagePin, &acknowledgeFlag};
    measurementTCB.task         = &measurementTask;
    measurementTCB.taskDataPtr  = &measure;
    measurementTCB.next         = &alarmTCB;
    measurementTCB.prev         = &touchInputTCB;

    display                 = {&HVIL, &temperature, &hvCurrent, &hvVoltage, &stateCharge,
                               &oldTemp, &oldCurr, &oldVolt, &oldSoc, &oldHVIL,
    						   &hvilFlag, &tempFlag, &currentFlag, &voltageFlag, &socFlag,
							   &hvilAlarm, &currentAlarm, &voltageAlarm,
                               &currentAlarmFlag, &voltageAlarmFlag, &hvilAlarmFlag,
							   &contactState, &oldContact, &toggleFlag, &setMeasure,
							   &setAlarm, &setBattery, &screenNum, &acknowledgeFlag,
							   &acknowledgeButtonFlag, &acknowledgeButtonExistsFlag,
							   &oldHVILAlarm, &oldCurrAlarm, &oldVoltAlarm, &presentHVILAlarm,
							   &presentCurrAlarm, &presentVoltAlarm, &tft};
    displayTCB.task         = &displayTask;
    displayTCB.taskDataPtr  = &display;
    displayTCB.next         = &touchInputTCB;
    displayTCB.prev         = NULL;

    // Initialize Touch Input
    touchInput                 = {&screenNum, &toggleFlag, &setMeasure, &setAlarm,
                                  &setBattery, &ts, &acknowledgeFlag, &acknowledgeButtonFlag, &tft};
    touchInputTCB.task         = &touchInputTask;
    touchInputTCB.taskDataPtr  = &touchInput;
    touchInputTCB.next         = &measurementTCB;
    touchInputTCB.prev         = &displayTCB;

    // Initialize Contactor
    contactor                 = {&contactState, &toggleFlag, &hvilAlarm, &contactPin, &HVIL};
    contactorTCB.task         = &contactorTask;
    contactorTCB.taskDataPtr  = &contactor;
    contactorTCB.next         = &socTCB;
    contactorTCB.prev         = &alarmTCB;

    // Initialize Alarm
    alarm                 = {&hvilAlarm, &currentAlarm, &voltageAlarm,
                             &currentAlarmFlag, &voltageAlarmFlag, &hvilAlarmFlag,
                             &HVIL, &hvCurrent, &hvVoltage};
    alarmTCB.task         = &alarmTask;
    alarmTCB.taskDataPtr  = &alarm;
    alarmTCB.next         = &contactorTCB;
    alarmTCB.prev         = &measurementTCB;

    // Initialize SOC
    soc                 = {&socFlag};
    socTCB.task         = &socTask;
    socTCB.taskDataPtr  = &soc;
    socTCB.next         = NULL;
    socTCB.prev         = &contactorTCB;


    // Initialize serial communication
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial1.setTimeout(1000);
	
    Timer1.initialize(100E+3);          // Set the timer period to 100E+3 uS (100mS)
    Timer1.attachInterrupt(timerISR);   // Attach the interrupt service routine (ISR)
    Timer1.start();                     // Start the timer
}

void loop() {
  /****************
    * Function name:    loop
    * Function inputs:  N/A
    * Function outputs: N/A
    * Function description: This is a synced time base that runs at 10 Hz to execute round robin scheduler
    * Author(s): 
    *****************/

    if (1 == timerFlag) {
        timerFlag = 0;
        scheduler(&displayTCB, &taskFlag);
        Serial.println("ok");
    }
}

// interrupt service routine
void timerISR() {
    timerFlag = 1;      // set timerISR flag
    return;
}
