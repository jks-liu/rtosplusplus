/* ************************************************
 * Copyright (c) 2013, jks Liu.
 * All rights reserved
 * **********************************************/



#include "../RtosPlusPlus.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


void *Task0(void *a)
{
    while(1){
        PORTC ^= 0x08;
        _delay_ms(100);
    }
    return 0;
}

void *Task1(void *a)
{
    while(1)
    {
        PORTC ^= 0x01;
        _delay_ms(100);
    }
    return 0;
}

void timer_init(void)
{
        TIMSK2 = _BV(OCIE0A);  // Enable Interrupt TimerCounter2 Compare Match A (SIG_OUTPUT_COMPARE2A)
        TCCR2A = _BV(WGM01);  // Mode = CTC
        TCCR2B = _BV(CS22);   // 8mHz / 64
        OCR2A = 55;        /* 2272 ticks per sec */
}

int main(int argc, char *argv[])
{
  timer_init();
  DDRC |= 0x09;   
  PORTC |= 0x08;
  char a[100];
  char a1[100];
  RtosPlusPlus::TCB b, b1;
  b.stack_top = (unsigned int)(a + 99);
  b1.stack_top = (unsigned int)(a1 + 99);
  b.start_routine = Task0;
  b1.start_routine = Task1;
  b.priority = 0;
  b1.priority = 0;
  
  ospp.create(&b);
  ospp.create(&b1);
  sei();
  while (1) {
    
  }
  return 0;
}

