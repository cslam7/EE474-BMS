#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library

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

TCB* taskQueue[] = {&displayTCB, &touchInputTCB, &measurementTCB, &alarmTCB, &contactorTCB, &socTCB};				// Declare array of TCBS to hold task functions

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
bool HVIL = TRUE;               // set HVIL output to initial state
const byte hvilPin  = 22;       // input pin via ATMega board
bool hvilFlag = TRUE;           // will update hvil on display
bool tempFlag = TRUE;           // will update temp on display 
bool currentFlag= TRUE;         // will update current on display
bool voltageFlag = TRUE;        // will update voltage on display

float hvilAlarm = 0;            // set initial state for hvil alarm
float currentAlarm = 0;         // set initial state for current alarm
float voltageAlarm = 0;         // set initial state for voltage alarm

bool hvilAlarmFlag = TRUE;      // will update hvil alarm on display
bool currentAlarmFlag = TRUE;   // will update current alarm on display
bool voltageAlarmFlag = TRUE;   // will update voltage alarm on display

float stateCharge = 0;          // set initial state for the SOC
bool socFlag = TRUE;            // will update SOC on display

bool contactState = TRUE;       // set initial state for the contactor
bool toggleFlag = FALSE;        // will not toggle contactor state on start up

bool setMeasure = TRUE;         // updates formatting for measurement screen on display
bool setAlarm = TRUE;           // updates formatting for alarm scren on display
bool setBattery = TRUE;         // updates formatting on battery on/off screen on display

int timeCount = 1;              // global time counter that syncs up variable cycling

int screenNum = 1;              // screen number {measurement = 1, alarm = 2, battery on/off = 3}

void setup() {  
  /****************
    * Function name: setup
    * Function inputs: None
    * Function outputs: None
    * Function description: sets up the scheduler and initiates LCD.
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
								   &timeCount};
    measurementTCB.task         = &measurementTask;
    measurementTCB.taskDataPtr  = &measure;
    measurementTCB.next         = NULL;
    measurementTCB.prev         = NULL;
     
    // Initialize Display
    display                 = {&HVIL, &temperature, &hvCurrent, &hvVoltage, &stateCharge,
    						   &hvilFlag, &tempFlag, &currentFlag, &voltageFlag, &socFlag,
							   &hvilAlarm, &currentAlarm, &voltageAlarm,
                               &currentAlarmFlag, &voltageAlarmFlag, &hvilAlarmFlag,
							   &contactState, &toggleFlag, &setMeasure,
							   &setAlarm, &setBattery, &toggleFlag, &screenNum, &tft};
    displayTCB.task         = &displayTask;
    displayTCB.taskDataPtr  = &display;
    displayTCB.next         = NULL;
    displayTCB.prev         = NULL;

    // Initialize Touch Input
    touchInput                 = {&screenNum, &toggleFlag, &setMeasure, &setAlarm, &setBattery, &ts, &tft};
    touchInputTCB.task         = &touchInputTask;
    touchInputTCB.taskDataPtr  = &touchInput;
    touchInputTCB.next         = NULL;
    touchInputTCB.prev         = NULL;

    // Initialize Contactor
    contactor                 = {&contactState, &toggleFlag};
    contactorTCB.task         = &contactorTask;
    contactorTCB.taskDataPtr  = &contactor;
    contactorTCB.next         = NULL;
    contactorTCB.prev         = NULL;

    // Initialize Alarm
    alarm                 = {&hvilAlarm, &currentAlarm, &voltageAlarm,
                             &currentAlarmFlag, &voltageAlarmFlag, &hvilAlarmFlag,
                             &timeCount};
    alarmTCB.task         = &alarmTask;
    alarmTCB.taskDataPtr  = &alarm;
    alarmTCB.next         = NULL;
    alarmTCB.prev         = NULL;

    // Initialize SOC
    soc                 = {&socFlag, &stateCharge, &timeCount};
    socTCB.task         = &socTask;
    socTCB.taskDataPtr  = &soc;
    socTCB.next         = NULL;
    socTCB.prev         = NULL;


    // Initialize serial communication
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial1.setTimeout(1000);
	
}

void loop() {
  /****************
    * Function name:    loop
    * Function inputs:  Sensor data, touch input
    * Function outputs: Display data and lights indicating alarm status, contactor status, sensor data, & state of charge
    * Function description: This is a round robin scheduler to run a series of tasks
    *                       that create a user interface to a battery management system
    * Author(s): 
    *****************/
    while(1){

        // cycles through task queue, one round robin every second
        unsigned long begin = millis();
        for (int i = 0; i < NUMTASKS; i++) {            
            if (taskFlag[i]) {
                (*(taskQueue[i])).task((*(taskQueue[i])).taskDataPtr);
            }
        }
        unsigned long end = millis();
        if(end < begin + 1000) {
            delay(1000 - (end - begin));
        }
        timeCount++;
        Serial.println(timeCount);
    }
}
