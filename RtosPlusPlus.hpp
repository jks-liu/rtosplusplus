#ifndef RTOSPLUSPLUS_H_
#define RTOSPLUSPLUS_H_

// #include "rtosplusplus-config.h"
#include "List.h"
#include <inttypes.h>

// Only 7(0..6) priorities can be used, the last one(7) is used
// for idle thread.
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
    
    // User must assign below three variables before they create a thread.
    unsigned int stack_top;
    void *(*start_routine)(void *);
    uint8_t priority;

    void *arg;
    Status status;
  };
  RtosPlusPlus(void);
  int create(TCB *thread);
  void dispatch(void);
private:
  /* Head of thread lists of 8 priorities. */
  List thread_heads_[OSPP_PRIORITIES_NUM];
  /* Running threads of each priority. */
  List::ListHead *running_threads_[OSPP_PRIORITIES_NUM];
  TCB *running_thread;
  /* idle_thread is the main function */
  TCB idle_thread;
  int thread_number_of_each_priority_[OSPP_PRIORITIES_NUM];
};

extern RtosPlusPlus ospp;

#endif /* RTOSPLUSPLUS_H_ */

