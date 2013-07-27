/* ************************************************
 * Copyright (c) 2013, jks Liu.
 * All rights reserved
 * **********************************************/

/* This file must be saved with chinese GB code
 * because of the chinese character. 
 *
 * You can edit it with unicode if you don't care 
 * garbled, and you cannot edit lines with chinese
 * characters. 
 *
 * Mabey escape characters will be better,
 * but i don't love it. :( */

#include "../RtosPlusPlus.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#ifdef nfaijsbnfuiadbsfiuasdfiu

unsigned char Stack[200]; 
unsigned char OSMapTbl[9]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x00};
register unsigned char OSRdyTbl asm("r2"); 
register unsigned char OSTaskRunningPrio asm("r3"); 
#define OS_TASKS 3
struct TaskCtrBlock 
{
    unsigned int OSTaskStackTop; 
    unsigned int OSWaitTick;
} TCB[OS_TASKS+1];

register unsigned char tempR4 asm("r4");
register unsigned char tempR5 asm("r5");
register unsigned char tempR6 asm("r6");
register unsigned char tempR7 asm("r7");
register unsigned char tempR8 asm("r8");
register unsigned char tempR9 asm("r9");
register unsigned char tempR10 asm("r10");
register unsigned char tempR11 asm("r11");
register unsigned char tempR12 asm("r12");
register unsigned char tempR13 asm("r13");
register unsigned char tempR14 asm("r14");
register unsigned char tempR15 asm("r15");
register unsigned char tempR16 asm("r16");
register unsigned char tempR17 asm("r17");

void OSTaskCreate(void (*Task)(void),unsigned char *Stack,unsigned char TaskID)
{
    unsigned char i;
    *Stack--=(unsigned int)Task; 
    *Stack--=(unsigned int)Task>>8;
    *Stack--=0x00; //R1 __zero_reg__
    *Stack--=0x00; //R0 __tmp_reg__
    *Stack--=0x80;
    for(i=0;i<14;i++) 
        *Stack--=i;
    TCB[TaskID].OSTaskStackTop=(unsigned int)Stack; 
    OSRdyTbl|=OSMapTbl[TaskID]; 
}

void OSStartTask(void)
{
    OSTaskRunningPrio=OS_TASKS;
    SP=TCB[OS_TASKS].OSTaskStackTop+17;
    __asm__ __volatile__( "reti" );
}

void OSSched(void)
{
    
    __asm__ __volatile__("PUSH __zero_reg__ "); //R1
    __asm__ __volatile__("PUSH __tmp_reg__ "); //R0
    __asm__ __volatile__("IN __tmp_reg__,__SREG__ "); 
    __asm__ __volatile__("PUSH __tmp_reg__ ");
    __asm__ __volatile__("CLR __zero_reg__ "); 
    __asm__ __volatile__("PUSH R18 ");
    __asm__ __volatile__("PUSH R19 ");
    __asm__ __volatile__("PUSH R20 ");
    __asm__ __volatile__("PUSH R21 ");
    __asm__ __volatile__("PUSH R22 ");
    __asm__ __volatile__("PUSH R23 ");
    __asm__ __volatile__("PUSH R24 ");
    __asm__ __volatile__("PUSH R25 ");
    __asm__ __volatile__("PUSH R26 ");
    __asm__ __volatile__("PUSH R27 ");
    __asm__ __volatile__("PUSH R30 ");
    __asm__ __volatile__("PUSH R31 ");
    __asm__ __volatile__("PUSH R28 "); 
    __asm__ __volatile__("PUSH R29 ");
    TCB[OSTaskRunningPrio].OSTaskStackTop=SP; 
    unsigned char OSNextTaskID; 
    for (OSNextTaskID = 0; 
            OSNextTaskID < OS_TASKS && !(OSRdyTbl & OSMapTbl[OSNextTaskID]);
            OSNextTaskID++);
    OSTaskRunningPrio = OSNextTaskID ;
    cli(); 
    SP=TCB[OSTaskRunningPrio].OSTaskStackTop;
    sei();
    
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
    __asm__ __volatile__("POP __tmp_reg__ "); 
    __asm__ __volatile__("OUT __SREG__,__tmp_reg__ "); //
    __asm__ __volatile__("POP __tmp_reg__ ");
    __asm__ __volatile__("POP __zero_reg__ "); 
}
void OSTimeDly(unsigned int ticks)
{
    if(ticks)
    {
        OSRdyTbl &=~OSMapTbl[OSTaskRunningPrio];
        TCB[OSTaskRunningPrio].OSWaitTick=ticks;
        OSSched(); 
    }
}


extern volatile  uint32_t g_jiffies;

/* timer2 */
ISR(TIMER2_COMPA_vect)
{
    ++g_jiffies; 
    if (0 == (g_jiffies % 22/*72*/)) {
        for(int i=0;i<OS_TASKS;i++) 
        {
            if(TCB[i].OSWaitTick)
            {
                TCB[i].OSWaitTick--;
                if(TCB[i].OSWaitTick==0) 
                {
                    OSRdyTbl |= OSMapTbl[i]; 
                }
            }
        }
    }
    
}
/*
SIGNAL(SIG_OVERFLOW0)
{
    unsigned char i;
    for(i=0;i<OS_TASKS;i++) 
        if(TCB[i].OSWaitTick)
        {
            TCB[i].OSWaitTick--;
            if(TCB[i].OSWaitTick==0) 
            {
                OSRdyTbl |= OSMapTbl[i]; 
            }
        }
    }
    TCNT0=100;
}*/
void Task2(void)
{
    while(1)
    {
        OSTimeDly(200);
    }
}
void TaskScheduler(void)
{
    while(1)
    {
        OSSched(); 
    }
}

int main1(void)
{
    TCN0Init();
    OSRdyTbl=0;
    OSTaskRunningPrio=0;
    OSTaskCreate(Task0,&Stack[49],0);
    OSTaskCreate(Task1,&Stack[99],1);
    OSTaskCreate(Task2,&Stack[149],2);
    OSTaskCreate(TaskScheduler,&Stack[199],OS_TASKS);
    
    sei();

//    Timer_ms t(500);
//    t.reset();

    DDRC |= 0x09;   

    OSStartTask();
    return 0;
}

#endif 
void *Task0(void *a)
{
    while(1){
        PORTC ^= 0x08;
        _delay_ms(200);
    }
    return 0;
}
void *Task1(void *a)
{
    while(1)
    {
        PORTC ^= 0x01;
        _delay_ms(200);
    }
    return 0;
}

void TCN0Init(void)
{
//    TCCR0 = 0;
//    TCCR0 |= (1<<CS02); 
//    TIMSK |= (1<<TOIE0); 
//    TCNT0 = 100; 
//    {
        /* timer2 */
        TIMSK2 = _BV(OCIE0A);  // Enable Interrupt TimerCounter2 Compare Match A (SIG_OUTPUT_COMPARE2A)
        TCCR2A = _BV(WGM01);  // Mode = CTC
        TCCR2B = _BV(CS22);   // 8mHz / 64
        OCR2A = 55;        /* 2272 ticks per sec */
//    }
}

int main(int argc, char *argv[])
{
  TCN0Init();
  DDRC |= 0x09;   
  char a[100];
  char a1[100];
  RtosPlusPlus::TCB b, b1;
  b.stack_top = (unsigned int)a;
  b1.stack_top = (unsigned int)a1;
  b.start_routine = Task0;
  b1.start_routine = Task1;
  
  ospp.create(&b);
  ospp.create(&b1);
  sei();
  return 0;
}

  

int main0(void)
{
    sei();

//    Timer_ms t(500);
//    t.reset();

    DDRC |= 0x09;    
    
    while (1) {
//        if (t.is_timeout()) {
//            t.reset();
//            PORTC ^= 0x01;
//        }
    }

    return 0;
}




