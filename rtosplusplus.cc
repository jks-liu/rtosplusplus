#include "rtosplusplus.h"
#include <avr/io.h>
#include <avr/Interrupt.h>
#include <inttypes.h>

static uint8_t register g_ready_thread asm("r2");
static uint8_t register g_runing_thread asm("r3");

static void(*g_hook)(RtosPlusPlus *);

RtosPlusPlus::RtosPlusPlus(void(*hook)(RtosPlusPlus *)):
  threads_num_(0)
{
  g_hook = hook;
}

RtosPlusPlus::creat(RtosPlusPlus::TCB const &thread)
{
  memcpy(&tcbs_[threads_num_++], &thread, sizeof(thread));
}



