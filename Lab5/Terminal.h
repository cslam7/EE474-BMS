#ifndef TERMINAL_H_
#define TERMINAL_H_

#include <stdlib.h>
#include <stdbool.h>
//#include <stdio.h>
#include <Arduino.h>

#define TRUE 1
#define FALSE 0

#define OPTION_1 1
#define OPTION_2 2
#define OPTION_3 3
#define OPTION_4 4

typedef struct terminalTaskData {

    bool* terminalPromptFlag;     // flag that determiens whether to display display prompt
    float *minCurrent;            // minimum value for hvCurrent in EEPROM
    float *maxCurrent;            // max value for hvCurrent in EEPROM     

    float *minVoltage;            // minimum value for hvVoltage in EEPROM
    float *maxVoltage;            // max value for hvVoltage in EEPROM

    float *minTemp;               // minimum value for temperature in EEPROM
    float *maxTemp;               // max value for temperature in EEPROM

    bool *resetFlag;              // flag for setting all EEPROM values to default value

} terminalData;  


void terminalTask(void* mData);

void updateTerminal(bool *resetFlagPtr, float *minCurrentPtr, float *maxCurrentPtr,
                    float *minVoltagePtr, float *maxVoltagePtr, float *minTempPtr,
                    float *maxTempPtr, bool *terminalPromptFlagPtr);

#endif
