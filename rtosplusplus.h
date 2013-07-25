#ifndef RTOSPLUSPLUS_H_
#define RTOSPLUSPLUS_H_

/* **********************************************************************
 * There are 7 priorities. 
 * ***********************************************************************/

// #include "rtosplusplus-config.h"
#include "list.h"
#include <inttypes.h>

extern RtosPlusPlus ospp;

class RtosPlusPlus {
public:
  struct TCB { // Thread Control Block
    /* The stack grow down, stack_top points invalid data. */
    List::ListHead node;
    unsigned int stack_top;
    void *(*start_routine)(void *);
    void *arg
    uint8_t priority;
  };
  RtosPlusPlus(void(*hook)(RtosPlusPlus *));
  int create(TCB *thread);
private:
  List thread_list_[8];
  TCB *running_thread;
  TCB idle_thread;
};

#endif /* RTOSPLUSPLUS_H_ */

