//--------------------------------------------------------------
// copyright (c) 2013 jks Liu.
// All rights reserved.
// -------------------------------------------------------------

#include "my_interrupt.hpp"

volatile  uint32_t g_jiffies = 0;

uint32_t get_time_ms(void)
{
    return g_jiffies / 22 * 10;
}





