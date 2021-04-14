#include "DataLogging.h"
void writeEEPROM(int addressBegin, float *data) {
    int floatSize = sizeof(*data);
    byte dataArray[floatSize];

    memcpy(&dataArray, data, floatSize);

    int i;
    int arrayCounter = 0;
    for (i = addressBegin; i < addressBegin + floatSize; i++) {
        EEPROM.write(i, dataArray[arrayCounter]);
        arrayCounter++;
    }

    return;
}


void readEEPROM(int addressBegin, float *data) {
    int floatSize = sizeof(*data);
    byte dataArray[floatSize];

//    memcpy(data, dataArray, floatSize);

    int i;
    int arrayCounter = 0;
    for (i = addressBegin; i < addressBegin + floatSize; i++) {
        dataArray[arrayCounter] = EEPROM.read(i);
        arrayCounter++;
//        Serial.println(dataArray[arrayCounter]);
    }
    memcpy(data, &dataArray, floatSize);

    return;
}


void updateLog(bool *resetFlagPtr, float *minCurrentPtr, bool *minCurrentFlagPtr,
               float *maxCurrentPtr, bool *maxCurrentFlagPtr, float *minVoltagePtr,
               bool *minVoltageFlagPtr, float *maxVoltagePtr, bool *maxVoltageFlagPtr,
               float *minTempPtr, bool *minTempFlagPtr, float *maxTempPtr, bool *maxTempFlagPtr) {
    /****************
    * Function name: 			updateLog
    * Function inputs: 			historical min and max values for current, voltage, and temperature with flags
    * Function outputs: 		updated EEPROM values for historial 
    * Function description: 	calls functions to updates LCD display
    *                           depending on variable flags
    * Author(s): 
    *****************/

    float resetCurrent = 0.0;
    float resetVoltage = -1.0;
    float resetTemperature = 0.0;

//    int temp1, temp2;
    noInterrupts();
    if (*resetFlagPtr == TRUE) {
        writeEEPROM(MIN_CURRENT_ADDRESS, &resetCurrent);
//        readEEPROM(MIN_CURRENT_ADDRESS, &resetCurrent);
       
        writeEEPROM(MAX_CURRENT_ADDRESS, &resetCurrent);
        writeEEPROM(MIN_VOLTAGE_ADDRESS, &resetVoltage);
//        readEEPROM(MIN_VOLTAGE_ADDRESS, &temp1);
        writeEEPROM(MAX_VOLTAGE_ADDRESS, &resetVoltage);
//        readEEPROM(MAX_VOLTAGE_ADDRESS, &temp2);

//        Serial.println(temp1); Serial.println(temp2);
        writeEEPROM(MIN_TEMP_ADDRESS, &resetTemperature);
        writeEEPROM(MAX_TEMP_ADDRESS, &resetTemperature);
        *resetFlagPtr = FALSE;

    } else {

        if (*minCurrentFlagPtr == TRUE) {
            writeEEPROM(MIN_CURRENT_ADDRESS, minCurrentPtr);
            *minCurrentFlagPtr = FALSE;
//            Serial.println("HERE");
        }
        if (*maxCurrentFlagPtr == TRUE) {
            writeEEPROM(MAX_CURRENT_ADDRESS, maxCurrentPtr);
            *maxCurrentFlagPtr = FALSE;
        }

        if (*minVoltageFlagPtr == TRUE) {
            writeEEPROM(MIN_VOLTAGE_ADDRESS, minVoltagePtr);
            *minVoltageFlagPtr = FALSE;
        }
        if (*maxVoltageFlagPtr == TRUE) {
            writeEEPROM(MAX_VOLTAGE_ADDRESS, maxVoltagePtr);
            *maxVoltageFlagPtr = FALSE;
        }

        if (*minTempFlagPtr == TRUE) {
            writeEEPROM(MIN_TEMP_ADDRESS, minTempPtr);
            *minTempFlagPtr = FALSE;
        }
        if (*maxTempFlagPtr == TRUE) {
            writeEEPROM(MAX_TEMP_ADDRESS, maxTempPtr);
            *maxTempFlagPtr = FALSE;
        }
    }
    interrupts();
    return;
}

void dataLogTask(void* mData) {
    /****************
    * Function name: 			dataLogTask
    * Function inputs: 			pointers in data log array
    * Function outputs: 		updates EEPROM values if desired
    * Function description: 	calls functions to update EEPROM
    * Author(s): 
    *****************/
  	dataLogData* data = (dataLogData*) mData;
  	updateLog(data->resetFlag, data->minCurrent, data->minCurrentFlag, data->maxCurrent,
              data->maxCurrentFlag, data->minVoltage,data->minVoltageFlag, data->maxVoltage,
              data->maxVoltageFlag, data->minTemp, data->minTempFlag, data->maxTemp, data->maxTempFlag);
	return;
}
