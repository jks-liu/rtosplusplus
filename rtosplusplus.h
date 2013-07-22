#ifndef RTOSPLUSPLUS_H_
#define RTOSPLUSPLUS_H_

class RtosPlusPlus {
public:
  struct Thread {
    /* The stack grow down, stack_top points invalid data. */
    unsigned int stack_top;
  };
  RtosPlusPlus(void);
  int create(Thread *t,
             const int /* must be 0*/,
             void *(*start_routine)(void *),
             void *arg);
};

#endif /* RTOSPLUSPLUS_H_ */

