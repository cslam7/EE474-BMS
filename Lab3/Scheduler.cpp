#include "Scheduler.h"
#include "TaskControlBlock.h"
//#include Lab3.ino
void scheduler(void* mData1, void* mData2) {
    /****************
    * Function name:            scheduler
    * Function inputs:          Pointer to linked list of TCBs
    * Function outputs:         Nothing
    * Function description:     Iterates through one round of tass and execute 
    *                           desired TCB.tasks
    * Author(s): 
    *****************/

  	TCB* dataPtr = (TCB*) mData1;
    bool* taskFlags = (bool*) mData2;
    TCB* starter = dataPtr;
    int i = 0;

    for (i = 0; i < NUMTASKS; i++) {            
        if (taskFlags[i]) {
            (*dataPtr).task((*dataPtr).taskDataPtr);
        }
        dataPtr = (*dataPtr).next;

    }
    dataPtr = starter;
	return;
}
