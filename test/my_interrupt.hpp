//--------------------------------------------------------------
// copyright (c) 2013 jks Liu.
// All rights reserved.
// -------------------------------------------------------------
#ifndef MY_INTERRUPT_H
#define MY_INTERRUPT_H

#include <avr/io.h>
#include <avr/interrupt.h>

uint32_t get_time_ms(void);

class Jiffies {
public:
    Jiffies(void) {
        /* timer2 */
        TIMSK2 = _BV(OCIE0A);  // Enable Interrupt TimerCounter2 Compare Match A (SIG_OUTPUT_COMPARE2A)
        TCCR2A = _BV(WGM01);  // Mode = CTC
        TCCR2B = _BV(CS22);   // 8mHz / 64
        OCR2A = 55;        /* 2272 ticks per sec */
    }
};

#endif /* MY_INTERRUPT_H */

