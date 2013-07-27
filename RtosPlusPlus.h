#ifndef RTOSPLUSPLUS_H_
#define RTOSPLUSPLUS_H_

/* **********************************************************************
 * There are 7 priorities. 
 * ***********************************************************************/

// #include "rtosplusplus-config.h"
#include "List.h"
#include <inttypes.h>


#define OSPP_PRIORITIES_NUM 8

class RtosPlusPlus {
public:
  enum Status {
    kRunning,
    kSuspend,
    kReady
  };
  struct TCB { // Thread Control Block
    /* The stack grow down, stack_top points invalid data. */
    List::ListHead node;
    unsigned int stack_top;
    void *(*start_routine)(void *);
    void *arg;
    uint8_t priority;
    Status status;
  };
  RtosPlusPlus(void(*hook)(RtosPlusPlus *));
  int create(TCB *thread);
  void dispatch(void);
private:
  /* Head of thread lists of 8 priorities. */
  List thread_heads_[OSPP_PRIORITIES_NUM];
  /* Running threads of each priority. */
  List *running_threads_[OSPP_PRIORITIES_NUM];
  TCB *running_thread;
  /* idle_thread is the main function */
  TCB idle_thread;
};

extern RtosPlusPlus ospp;

#endif /* RTOSPLUSPLUS_H_ */

