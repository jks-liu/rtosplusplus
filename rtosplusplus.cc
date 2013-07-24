#include "rtosplusplus.h"
#include <avr/io.h>
#include <avr/Interrupt.h>
#include <inttypes.h>

static uint8_t register g_ready_thread asm("r2");
static uint8_t register g_runing_thread asm("r3");
static uint8_t register g_existing_thread asm("r4");

static void(*g_hook)(RtosPlusPlus *);

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

__attribute__((naked)) static void
push_stack_like_interrupt(void) {
  asm volatile("push __zero_reg__"          "\n\t"); // r1
  asm volatile("push __tmp_reg__"           "\n\t");  // r0
  // statement register
  asm volatile("in   __tmp_reg__, __SREG__" "\n\t");
  asm volatile("push __tmp_reg__"           "\n\t");
  asm volatile("clr  __zero_reg__"          "\n\t"); 
  asm volatile("push r18"                   "\n\t");
  asm volatile("push r19"                   "\n\t");
  asm volatile("push r20"                   "\n\t");
  asm volatile("push r21"                   "\n\t");
  asm volatile("push r22"                   "\n\t");
  asm volatile("push r23"                   "\n\t");
  asm volatile("push r24"                   "\n\t");
  asm volatile("push r25"                   "\n\t");
  asm volatile("push r26"                   "\n\t");
  asm volatile("push r27"                   "\n\t");
  asm volatile("push r30"                   "\n\t");
  asm volatile("push r31"                   "\n\t");
}

static void switch_thread(void) {
  push_stack_like_interrupt();
  asm volatile("switch_thread_from_interrupt:" "\n\t");
    __asm__ __volatile__("PUSH R28 ");
    __asm__ __volatile__("PUSH R29 ");
    TCB[OSTaskRunningPrio].OSTaskStackTop=SP;
    unsigned char OSNextTaskID;
    for (OSNextTaskID = 0;
            OSNextTaskID < OS_TASKS && !(OSRdyTbl & OSMapTbl[OSNextTaskID]);
            OSNextTaskID++);
    OSTaskRunningPrio = OSNextTaskID ;
    cli(); // 保护堆栈转换
    SP=TCB[OSTaskRunningPrio].OSTaskStackTop;
    sei();
    // 根据中断时的出栈次序
    __asm__ __volatile__("POP R29 ");
    __asm__ __volatile__("POP R28 ");
    __asm__ __volatile__("POP R31 ");
    __asm__ __volatile__("POP R30 ");
    __asm__ __volatile__("POP R27 ");
    __asm__ __volatile__("POP R26 ");
    __asm__ __volatile__("POP R25 ");
    __asm__ __volatile__("POP R24 ");
    __asm__ __volatile__("POP R23 ");
    __asm__ __volatile__("POP R22 ");
    __asm__ __volatile__("POP R21 ");
    __asm__ __volatile__("POP R20 ");
    __asm__ __volatile__("POP R19 ");
    __asm__ __volatile__("POP R18 ");
    __asm__ __volatile__("POP __tmp_reg__ "); //SERG 出栈并恢复
    __asm__ __volatile__("OUT __SREG__,__tmp_reg__ "); //
    __asm__ __volatile__("POP __tmp_reg__ "); //R0 出栈
    __asm__ __volatile__("POP __zero_reg__ "); //R1 出栈// 中断时出栈完成
}
  

RtosPlusPlus::RtosPlusPlus(void(*hook)(RtosPlusPlus *)):
  threads_num_(0)
{
  g_hook = hook;
  g_runing_thread
}

RtosPlusPlus::creat(RtosPlusPlus::TCB const &thread)
{
  memcpy(&tcbs_[threads_num_++], &thread, sizeof(thread));
}



