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
#include "DataLogging.h"
#include "Terminal.h"

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
TCB dataLogTCB;             // Declare data logging TCB
TCB terminalTCB;            // Declare terminal TCB

TCB* taskQueue[] = {&touchInputTCB, &measurementTCB, &alarmTCB, &contactorTCB, &socTCB, &displayTCB, &terminalTCB, &dataLogTCB};				// Declare array of TCBS to hold task functions

measurementData measure;     // Declare measurement data structure - defined in Measurement.h
displayData display; 		 // Declare display data structure - defined in Display.h
touchInputData touchInput;	 // Declare touchinput data structure - defined in TouchInput.h
alarmData alarm;             // Declare alarm data structure - defined in Alarm.h
contactorData contactor;     // Declare contactor data structure - defined in Contactor.h
socData soc;                 // Declare state of charge data structure - defined in StateofCharge.h
dataLogData dataLog;         // Declare data log data structure - defined in DataLogging.h
terminalData terminal;       // Declare terminal data structure - defined in Terminal.h

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
bool historicalToggleFlag = FALSE;       // preserve state of toggle when display task isn't running    
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

volatile int global_counter = 0;       // global time counter for debugging

float oldTemp = -10;                // temperature to compare with for display updates            
float oldCurr = -20;                // current to compare with for display updates 
float oldVolt = 10;                 // voltage to compare with for display updates 
float oldSoc = 0;                   // soc to compare with for display updates 
float oldHVIL = FALSE;;             // hvil to compare with for display updates 

char *oldHVILAlarm;                 // old hvilAlarm to compare with for display updates
char *oldCurrAlarm;                 // old currAlarm to compare with for display updates
char *oldVoltAlarm;                 // old voltAlarm to compre with for display updates
char *presentHVILAlarm;             // present hvilAlarm to compare with for display updates
char *presentCurrAlarm;             // present currAlarm to compare with for display updates
char *presentVoltAlarm;             // present voltAlarm to compare with for display updates

// for floats below, initialize in setup with EEPROM.read / EEPROM.get
float minCurrent;               // minimum value for hvCurrent in EEPROM
bool minCurrentFlag = FALSE;        // flag for updating min current in EEPROM
float maxCurrent;               // max value for hvCurrent in EEPROM     
bool maxCurrentFlag = FALSE;        // flag for updating max current in EEPROM

float minVoltage;              // minimum value for hvVoltage in EEPROM
bool minVoltageFlag = FALSE;        // flag for updating min voltage in EEPROM
float maxVoltage;              // max value for hvVoltage in EEPROM
bool maxVoltageFlag = FALSE;        // flag for updating max voltage in EEPROM

float minTemp;                  // minimum value for temperature in EEPROM
bool minTempFlag = FALSE;           // flag for updating min temp in EEPROM
float maxTemp;                  // max value for temperature in EEPROM
bool maxTempFlag = FALSE;           // flag for updating max temp in EEPROM

bool terminalPrompt;                // flag to display terminal prompt again
bool resetFlag = FALSE;             // user triggered flag to reset value ranges

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
								   &tempPin, &currentPin, &voltagePin, &acknowledgeFlag,
                                   &minCurrent, &minCurrentFlag, &maxCurrent, &maxCurrentFlag,
                                   &minVoltage, &minVoltageFlag, &maxVoltage, &maxVoltageFlag,
                                   &minTemp, &minTempFlag, &maxTemp, &maxTempFlag, &resetFlag};
    measurementTCB.task         = &measurementTask;
    measurementTCB.taskDataPtr  = &measure;
    measurementTCB.next         = &alarmTCB;
    measurementTCB.prev         = &touchInputTCB;

    display                     = {&HVIL, &temperature, &hvCurrent, &hvVoltage, &stateCharge,
                                   &oldTemp, &oldCurr, &oldVolt, &oldSoc, &oldHVIL,
                                   &hvilFlag, &tempFlag, &currentFlag, &voltageFlag, &socFlag,
                                   &hvilAlarm, &currentAlarm, &voltageAlarm,
                                   &currentAlarmFlag, &voltageAlarmFlag, &hvilAlarmFlag,
                                   &contactState, &oldContact, &toggleFlag, &historicalToggleFlag,
                                   &setMeasure, &setAlarm, &setBattery, &screenNum, &acknowledgeFlag,
                                   &acknowledgeButtonFlag, &acknowledgeButtonExistsFlag,
                                   &oldHVILAlarm, &oldCurrAlarm, &oldVoltAlarm, &presentHVILAlarm,
                                   &presentCurrAlarm, &presentVoltAlarm, &tft};
    displayTCB.task         = &displayTask;
    displayTCB.taskDataPtr  = &display;
    displayTCB.next         = NULL;
    displayTCB.prev         = NULL;

    // Initialize Touch Input
    touchInput                 = {&screenNum, &toggleFlag, &historicalToggleFlag, &setMeasure, &setAlarm,
                                  &setBattery, &ts, &acknowledgeFlag, &acknowledgeButtonFlag, &tft};
    touchInputTCB.task         = &touchInputTask;
    touchInputTCB.taskDataPtr  = &touchInput;
    touchInputTCB.next         = &measurementTCB;
    touchInputTCB.prev         = NULL;

    // Initialize Contactor
    contactor                  = {&contactState, &toggleFlag, &contactPin, &hvilAlarm,
                                  &currentAlarm, &voltageAlarm, &acknowledgeFlag, &historicalToggleFlag};
    contactorTCB.task          = &contactorTask;
    contactorTCB.taskDataPtr   = &contactor;
    contactorTCB.next          = &socTCB;
    contactorTCB.prev          = &alarmTCB;

    // Initialize Alarm
    alarm                 = {&hvilAlarm, &currentAlarm, &voltageAlarm,
                             &currentAlarmFlag, &voltageAlarmFlag, &hvilAlarmFlag,
                             &HVIL, &hvCurrent, &hvVoltage};
    alarmTCB.task         = &alarmTask;
    alarmTCB.taskDataPtr  = &alarm;
    alarmTCB.next         = &contactorTCB;
    alarmTCB.prev         = &measurementTCB;

    // Initialize SOC
    soc                   = {&stateCharge, &socFlag, &temperature, &tempFlag,
                            &hvCurrent, &currentFlag, &hvVoltage, &voltageFlag};
    socTCB.task           = &socTask;
    socTCB.taskDataPtr    = &soc;
    socTCB.next           = NULL;
    socTCB.prev           = &contactorTCB;

    // Initialize Data Log
    dataLog                 = {&resetFlag, &minCurrent, &minCurrentFlag,
                               &maxCurrent, &maxCurrentFlag, &minVoltage,
                               &minVoltageFlag, &maxVoltage, &maxVoltageFlag,
                               &minTemp, &minTempFlag, &maxTemp, &maxTempFlag};
    dataLogTCB.task         = &dataLogTask;
    dataLogTCB.taskDataPtr  = &dataLog;
    dataLogTCB.next         = NULL;
    dataLogTCB.prev         = NULL;

    // Initialize Remote Terminal
    terminal                = {&terminalPrompt, &minCurrent, &maxCurrent,
                               &minVoltage, &maxVoltage, &minTemp, &maxTemp,
                               &resetFlag};
    terminalTCB.task        = &terminalTask;
    terminalTCB.taskDataPtr = &terminal;
    terminalTCB.next        = NULL;
    terminalTCB.prev        = NULL;

    terminalPrompt = TRUE;              // sets terminal to only prmopt at startup

//    for (int i = 0 ; i < EEPROM.length() ; i++) {
//        EEPROM.put(i, 20.0);
//    }
//    float i = 20.0;
//    writeEEPROM(MIN_VOLTAGE_ADDRESS, &i);    // read EEPROM value into respective historical minVoltage
//    Serial.println(minVoltage);
//    writeEEPROM(MAX_VOLTAGE_ADDRESS, &i);    // read EEPROM value into respective historical maxVoltage

//    Serial.println("starting reading");
    readEEPROM(MIN_CURRENT_ADDRESS, &minCurrent);    // read EEPROM value into respective historical minCurrent
    readEEPROM(MAX_CURRENT_ADDRESS, &maxCurrent);    // read EEPROM value into respective historical maxCurrent
    
    readEEPROM(MIN_VOLTAGE_ADDRESS, &minVoltage);    // read EEPROM value into respective historical minVoltage
//    Serial.println(minVoltage);
    readEEPROM(MAX_VOLTAGE_ADDRESS, &maxVoltage);    // read EEPROM value into respective historical maxVoltage

    readEEPROM(MIN_TEMP_ADDRESS, &minTemp);          // read EEPROM value into respective minTemp
    readEEPROM(MAX_TEMP_ADDRESS, &maxTemp);          // read EEPROM value into respective maxTemp

//    Serial.println(minVoltage);
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial1.setTimeout(1000);
	
    Timer1.initialize(100E+3);          // Set the timer period to 100E+3 uS (100mS)
    Timer1.attachInterrupt(timerISR);   // Attach the interrupt service routine (ISR)
    Timer1.start();                     // Start the timer
//    Serial.print("min curr   "); Serial.println(minCurrent);
//    Serial.print("max curr   "); Serial.println(maxCurrent);
//    Serial.print("min volt   "); Serial.println(minVoltage);
//    Serial.print("max volt   "); Serial.println(maxVoltage);
//    Serial.print("min temp   "); Serial.println(minTemp);
//    Serial.print("max temp   "); Serial.println(maxTemp);
}

void loop() {
  /****************
    * Function name:    loop
    * Function inputs:  N/A
    * Function outputs: N/A
    * Function description: This is a synced time base that runs at 10 Hz to execute round robin scheduler
    * Author(s): 
    *****************/
//    if (global_counter == 0) {
//        Serial.println("startreading");
////        float temp = 20.0;
////        writeEEPROM(MIN_VOLTAGE_ADDRESS, &temp);
//        readEEPROM(MIN_CURRENT_ADDRESS, &minCurrent);    // read EEPROM value into respective historical minCurrent
//        readEEPROM(MAX_CURRENT_ADDRESS, &maxCurrent);    // read EEPROM value into respective historical maxCurrent
//
//        readEEPROM(MIN_VOLTAGE_ADDRESS, &minVoltage);    // read EEPROM value into respective historical minVoltage
//    //    Serial.println(minVoltage);
//        readEEPROM(MAX_VOLTAGE_ADDRESS, &maxVoltage);    // read EEPROM value into respective historical maxVoltage
////        Serial.println(maxVoltage);
////        Serial.println(minVoltage);
//        readEEPROM(MIN_TEMP_ADDRESS, &minTemp);          // read EEPROM value into respective minTemp
//        readEEPROM(MAX_TEMP_ADDRESS, &maxTemp);          // read EEPROM value into respective maxTemp
//    }
    if (1 == timerFlag) {
        timerFlag = 0;
        scheduler(&touchInputTCB, taskQueue);
//         Serial.println("ok");
//        Serial.println(resetFlag);
    }
}

// interrupt service routine
void timerISR() {
    timerFlag = 1;      // set timerISR flag
    return;
}
