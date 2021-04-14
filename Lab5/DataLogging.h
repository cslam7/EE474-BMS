#ifndef DATALOGGING_H_
#define DATALOGGING_H_
#include <EEPROM.h>

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>

#define TRUE 1
#define FALSE 0

#define MIN_CURRENT_ADDRESS  0
#define MAX_CURRENT_ADDRESS 10
#define MIN_VOLTAGE_ADDRESS 20
#define MAX_VOLTAGE_ADDRESS 30
#define MIN_TEMP_ADDRESS 40
#define MAX_TEMP_ADDRESS 50

typedef struct dataLogData {

    bool *resetFlag;        // user request of EEPROM values

    float *minCurrent;      // minimum value for hvCurrent in EEPROM
    bool *minCurrentFlag;   // flag for updating min current in EEPROM
    float *maxCurrent;      // max value for hvCurrent in EEPROM     
    bool *maxCurrentFlag;   // flag for updating max current in EEPROM

    float *minVoltage;      // minimum value for hvVoltage in EEPROM
    bool *minVoltageFlag;   // flag for updating min voltage in EEPROM
    float *maxVoltage;      // max value for hvVoltage in EEPROM
    bool *maxVoltageFlag;   // flag for updating max voltage in EEPROM

    float *minTemp;         // minimum value for temperature in EEPROM
    bool *minTempFlag;      // flag for updating min temp in EEPROM
    float *maxTemp;         // max value for temperature in EEPROM
    bool *maxTempFlag;      // flag for updating max temp in EEPROM

} dataLogData;

void dataLogTask (void* mData);

void updateLog(bool *resetFlagPtr, float *minCurrentPtr, bool *minCurrentFlagPtr,
               float *maxCurrentPtr, bool *maxCurrentFlagPtr, float *minVoltagePtr,
               bool *minVoltageFlagPtr, float *maxVoltagePtr, bool *maxVoltageFlagPtr,
               float *minTempPtr, bool *minTempFlagPtr, float *maxTempPtr, bool *maxTempFlagPtr);

void writeEEPROM(int addressBegin, float *data);

void readEEPROM(int addressBegin, float *data);


#endif
