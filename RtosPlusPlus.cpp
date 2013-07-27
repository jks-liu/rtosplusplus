#include "RtosPlusPlus.hpp"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

// Definition with asm MUST NOT be static
uint8_t register g_running_priority asm("r2");
uint8_t register g_running_thread_in_a_priority asm("r3");
uint8_t register g_existing_thread asm("r4");

void(*g_hook)(RtosPlusPlus *);

// Bit0 has the highest priority.
register uint8_t g_ready_priorities asm("r5");

register uint8_t g_priority0_ready asm("r6");
register uint8_t g_priority1_ready asm("r7");
register uint8_t g_priority2_ready asm("r8");
register uint8_t g_priority3_ready asm("r9");
register uint8_t g_priority4_ready asm("r10");
register uint8_t g_priority5_ready asm("r11");
register uint8_t g_priority6_ready asm("r12");
register uint8_t g_priority7_ready asm("r13");

register uint8_t g_unuse14 asm("r14");
register uint8_t g_unuse15 asm("r15");
register uint8_t g_CANNOTUSE asm("r16");
register uint8_t g_unuse17 asm("r17");

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

__attribute__((used)) static void dispatch_thread(void) {
  push_stack_like_interrupt();
  asm volatile("dispatch_thread_from_interrupt:" "\n\t");
  asm volatile("push r28" "\n\t");
  asm volatile("push r29" "\n\t");
  
    // TCB[OSTaskRunningPrio].OSTaskStackTop=SP;
    // unsigned char OSNextTaskID;
    // for (OSNextTaskID = 0;
    //         OSNextTaskID < OS_TASKS && !(OSRdyTbl & OSMapTbl[OSNextTaskID]);
    //         OSNextTaskID++);
    // OSTaskRunningPrio = OSNextTaskID ;
    // cli(); 
    // SP=TCB[OSTaskRunningPrio].OSTaskStackTop;
    // sei();
  ospp.dispatch();
    
  asm volatile("pop r29"                  "\n\t");
  asm volatile("pop r28"                  "\n\t");
  asm volatile("pop r31"                  "\n\t");
  asm volatile("pop r30"                  "\n\t");
  asm volatile("pop r27"                  "\n\t");
  asm volatile("pop r26"                  "\n\t");
  asm volatile("pop r25"                  "\n\t");
  asm volatile("pop r24"                  "\n\t");
  asm volatile("pop r23"                  "\n\t");
  asm volatile("pop r22"                  "\n\t");
  asm volatile("pop r21"                  "\n\t");
  asm volatile("pop r20"                  "\n\t");
  asm volatile("pop r19"                  "\n\t");
  asm volatile("pop r18"                  "\n\t");
  asm volatile("pop __tmp_reg__"          "\n\t"); 
  asm volatile("out __SREG__,__tmp_reg__" "\n\t");
  asm volatile("pop __tmp_reg__"          "\n\t"); 
  asm volatile("pop __zero_reg__"         "\n\t"); 
}

ISR(TIMER2_COMPA_vect) {
  asm volatile("rjmp dispatch_thread_from_interrupt \n\r");
}
  

RtosPlusPlus::RtosPlusPlus(void(*hook)(RtosPlusPlus *)) {
  g_hook = hook;
  
  thread_heads_[OSPP_PRIORITIES_NUM - 1].add(&(idle_thread.node));
  running_thread = &idle_thread;
  idle_thread.status = kRunning;
}

int RtosPlusPlus::create(RtosPlusPlus::TCB *thread) {
  thread_heads_[thread->priority].add(&(thread->node));
  thread->status = kReady;
  running_threads_[thread->priority] = (List::ListHead *)thread;
  ++thread_number_of_each_priority_[thread->priority];
  uint8_t *stack = (uint8_t *)(thread->stack_top);
  // High 8 bits
  *stack-- = (uint8_t)((unsigned int)(thread->start_routine) >> 8);
  *stack-- = (uint8_t)(unsigned int)(thread->start_routine);  // Low 8 bits
  *stack-- = 0x00;      // r1 __zero_reg__
  *stack-- = 0x00;      // r0 __tmp_reg__
  *stack-- = 0x80;      // __SREG__
  stack -= 16;          // r18~r27, r30, r31, r28, r29, dispatch(2)
  thread->stack_top = (unsigned int)stack;
  return 0;
}

void RtosPlusPlus::dispatch(void) {
  running_thread->stack_top = SP;
  for (unsigned char i = 0; i < OSPP_PRIORITIES_NUM; ++i) {
    if (thread_number_of_each_priority_[i] != 0) {
      running_thread =(TCB *)(
          thread_heads_[i].is_last(running_threads_[i]) ?
          thread_heads_[i].get(0): running_threads_[i]->next);
      break;
    }
  }
  SP = running_thread->stack_top;
}
      
    
RtosPlusPlus ospp(0);
















