#include "Scheduler.h"
#include "Display.h"
#include "Measurement.h"
#include "Display.h"
#include "TouchInput.h"
#include "Terminal.h"
#include "DataLogging.h"

static TCB *head;
static TCB *tail;
extern volatile int global_counter;
static int task_counter = 5;
//#include Lab3.ino


// This function assumes that head and tail pointers have already been created and 
//are global and that the pointers contained in the TCB node have already been initialized to NULL 
// This function also assumes that the “previous” and “next” pointers in the TCB 
//node are called “prev” and “next” respectively */ 
void insertNode(TCB* node) { 
                                // If the head pointer is pointing to nothing
   if (head == NULL) {          // set the head and tail pointers to point to this node 
       head = node; 
       tail = node; 
   } else {                     // otherwise,add node to end of the list
       tail->next = node; 
       node->prev = tail;       // note that the head pointer has shifted to the most recently added node
       tail = node;
   }
   task_counter++;
//   Serial.println(global_counter);
   return;
}


// remove TCB from doubly linked list if needed
void removeNode(TCB* node) {

   if (head == tail) {             // if there is only one node, remove it from list
       head = NULL; 
       tail = NULL; 
   } else if (head == node) {      // if the head is the node, then move head up one place
       head = head->next;
       (head->prev)->next = NULL;
   } else if (tail == node) {      // if the tail is the node, then move tail back one place
       tail = tail->prev;
       tail->next = NULL; 
   } else {                        // if the node is in the middle of the list, then clear pointers
       (node->prev)->next = node->next; 
       (node->next)->prev = node->prev;
       node->prev = NULL;
       node->next = NULL; 
   }

   task_counter--;
   return;
}

// calls functions to insert respective nodes at given times
void fillSchedule(TCB* dataPtr, TCB* taskQueue[]) {
    
    if (global_counter % 10 == 0) {
        insertNode(taskQueue[5]);
        insertNode(taskQueue[6]);
    }
    if (global_counter % 50 == 0) {
        insertNode(taskQueue[7]);
    }

}

// calls functions to remove respective nodes at given times
void shrinkSchedule(TCB *dataPtr) {
    
    if (global_counter % 10 == 0) {
        removeNode(tail);
        removeNode(tail);
    }

    if (global_counter % 50 == 0) {
        removeNode(tail);
    }
}

void scheduler(TCB* dataPtr, TCB* taskQueue[]) {
    /****************
    * Function name:            scheduler
    * Function inputs:          Pointer to linked list of TCBs and filled task Queue
    * Function outputs:         Nothing
    * Function description:     Iterates through one round of scheduler and execute 
    *                           desired TCB.tasks
    * Author(s): 
    *****************/

    head = taskQueue[0];
    tail = taskQueue[4];
    fillSchedule(dataPtr, taskQueue);

    dataPtr = head;
    int i;
    for (i = 0; i < task_counter; i++) {
        (*dataPtr).task((*dataPtr).taskDataPtr);
        dataPtr = (*dataPtr).next;

    }

    shrinkSchedule(dataPtr);
    global_counter++;
	return;
}
