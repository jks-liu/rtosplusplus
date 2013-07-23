#ifndef RTOSPLUSPLUS_H_
#define RTOSPLUSPLUS_H_

#include "rtosplusplus-config.h"

class RtosPlusPlus {
public:
  struct TCB { // Thread Control Block
    /* The stack grow down, stack_top points invalid data. */
    unsigned int stack_top;
    void *(*start_routine)(void *);
    void *arg
  };
  RtosPlusPlus(void(*hook)(RtosPlusPlus *));
  int create(TCB const &thread);
private:
  uint8_t threads_num_;
  TCB tcbs_[THREADS_NUM];
};

#endif /* RTOSPLUSPLUS_H_ */

