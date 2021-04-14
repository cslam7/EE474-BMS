#include "Terminal.h"

void updateTerminal(bool *resetFlagPtr, float *minCurrentPtr, float *maxCurrentPtr,
                    float *minVoltagePtr, float *maxVoltagePtr, float *minTempPtr,
                    float *maxTempPtr, bool *terminalPromptFlagPtr) {

 /****************
    * Function name:            Terminal 
    * Function inputs:          Pointer to possible bounds for voltage, current, temperature and reset flags
    * Function outputs:         Outputs the commands and responses to serial monitor
    * Function description:     Controls the user interace via the Serial monitor
    * Author(s): 
    *****************/

    // Serial.print("should be 1"); Serial.println(*terminalPromptFlagPtr);
    // shows user prompt if first time booting on Serial monitor
    if (*terminalPromptFlagPtr == TRUE) {
        Serial.println("[1]  Reset EEPROM");
        Serial.println("[2]  HV Current Range [Hi, Lo]");
        Serial.println("[3]  HV Voltage Range [Hi, Lo]");
        Serial.println("[4]  Temperature Range [Hi, Lo]");
        Serial.println("Enter your menu choice [1-4]:");
        *terminalPromptFlagPtr = FALSE;
    }
    // Serial.print("should be 0"); Serial.println(*terminalPromptFlagPtr);

    int menuNumber = 0;
    // obtain user input as int
    // while(Serial.available() == 0){
    menuNumber = Serial.parseInt();
    // }
   
    // depending on user input, either reset or show historical range of current || voltage || temperature
    if (menuNumber == OPTION_1) {
        *resetFlagPtr = TRUE;
    } else if (menuNumber == OPTION_2) {
        Serial.print("The HV current range is [High: "); Serial.print(*maxCurrentPtr);
        Serial.print(", Low "); Serial.print(*minCurrentPtr); Serial.println("]");
    } else if (menuNumber == OPTION_3) {
        Serial.print("The HV voltage range is [High: "); Serial.print(*maxVoltagePtr);
        Serial.print(", Low "); Serial.print(*minVoltagePtr); Serial.println("]");
    } else if (menuNumber == OPTION_4) {
        Serial.print("The temperature range is [High: "); Serial.print(*maxTempPtr);
        Serial.print(", Low "); Serial.print(*minTempPtr); Serial.println("]");
    }

    menuNumber = 0; 
    return;
}

void terminalTask(void* mData) {
    /****************
    * Function name: 			TerminalData
    * Function inputs: 			pointers to terminal data
    * Function outputs: 		reset EEPROM values if desired and set flags for changed values
    * Function description: 	calls functions to update EEPROM or display EEPROM values
    * Author(s): 
    *****************/
  	terminalData* data = (terminalData*) mData;
  	updateTerminal(data->resetFlag, data->minCurrent, data->maxCurrent,
              data->minVoltage, data->maxVoltage,data->minTemp,
              data->maxTemp, data->terminalPromptFlag);

    return;
}
