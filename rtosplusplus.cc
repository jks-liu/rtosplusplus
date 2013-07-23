#include "rtosplusplus.h"
#include <avr/io.h>
#include <avr/Interrupt.h>
#include <inttypes.h>

static uint8_t register g_ready_thread asm("r2");
static uint8_t register g_runing_thread asm("r3");

static void(*g_hook)(RtosPlusPlus *);

static register uint8_t g_unuse4 asm("r4");
static register uint8_t g_unuse5 asm("r5");
static register uint8_t g_unuse6 asm("r6");
static register uint8_t g_unuse7 asm("r7");
static register uint8_t g_unuse8 asm("r8");
static register uint8_t g_unuse9 asm("r9");
static register uint8_t g_unuse10 asm("r10");
static register uint8_t g_unuse11 asm("r11");
static register uint8_t g_unuse12 asm("r12");
static register uint8_t g_unuse13 asm("r13");
static register uint8_t g_unuse14 asm("r14");
static register uint8_t g_unuse15 asm("r15");
static register uint8_t g_unuse16 asm("r16");
static register uint8_t g_unuse17 asm("r17");


RtosPlusPlus::RtosPlusPlus(void(*hook)(RtosPlusPlus *)):
  threads_num_(0)
{
  g_hook = hook;
}

RtosPlusPlus::creat(RtosPlusPlus::TCB const &thread)
{
  memcpy(&tcbs_[threads_num_++], &thread, sizeof(thread));
}



