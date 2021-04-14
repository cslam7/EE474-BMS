#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>


#define TRUE 1
#define FALSE 0

#define NUMSTATES 3

#define STATE1 0
#define STATE2 1
#define STATE3 2

#define OFF 0

#define VOLTTIMER 3
#define CURRTIMER 2

#define HVIL_OPEN   FALSE
#define HVIL_CLOSED TRUE

#define OPEN 1
#define CLOSE 0

typedef struct measurementTaskData {

    bool* hvilStatus;                                    // status of hvil from test circuit
    const byte* hvilPin;                                 // pin number for reading input for hvil
    float* temperature;                                  // present value of temp
    float* hvCurrent;                                    // present value of current
    float* hvVoltage;                                    // present value of voltage
    volatile bool *hvilFlag;                             // flag for updating hvil
    bool *tempFlag, *currentFlag, *voltageFlag;          // flags for updating fields on display
//    int* timeCount;                 // global time counter
    const byte *tempPin, *currentPin, *voltagePin;       // GPIO pins for contactor, temperature, current and voltage
    volatile bool* acknowledgeFlag;                      // whether an alarm is active but not acknowledged

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

    bool *resetFlag;        // flag for resetting all min/max values

} measurementData;

void measurementTask (void* mData);

void updateHVIL(bool* hvilReading, const byte* pin, bool* hvilFlagPtr, bool *acknowledgeFlag);

void updateTemperature(float* temperatureReading, bool* tempFlag, const byte *tempPin,
                       float *minTempPtr, bool *minTempFlagPtr, float *maxTempPtr,
                       bool *maxTempFlagPtr, bool *resetFlagPtr);

void updateHvCurrent(float* currentReading, bool* currentFlag, const byte *currentPin,
                     float *minCurrentPtr, bool *minCurrentFlagPtr, float *maxCurrentPtr,
                     bool *maxCurrentFlagPtr, bool *resetFlagPtr);

void updateHvVoltage(float* voltageReading, bool* voltageFlag, const byte *voltagePin,
                     float *minVoltagePtr, bool *minVoltageFlagPtr, float *maxVoltagePtr,
                     bool *maxCurrentFlagPtr, bool *resetFlagPtr);

#endif
