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

#include <avr/io.h>
#include <avr/Interrupt.h>
#include <util/delay.h>
unsigned char Stack[200]; // 建立一个 200 字节的人工堆栈
unsigned char OSMapTbl[9]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x00};// 建一个常数表
register unsigned char OSRdyTbl asm("r2"); // 任务运行就绪表
register unsigned char OSTaskRunningPrio asm("r3"); // 正在运行的任务
#define OS_TASKS 3 // 设定运行任务的数量
struct TaskCtrBlock // 任务控制块
{
    unsigned int OSTaskStackTop; // 保存任务的堆栈顶
    unsigned int OSWaitTick; // 任务延时时钟
} TCB[OS_TASKS+1];
// 防止被编译器占用
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
// 建立任务
void OSTaskCreate(void (*Task)(void),unsigned char *Stack,unsigned char TaskID)
{
    unsigned char i;
    *Stack--=(unsigned int)Task; // 将任务的地址低位压入堆栈，
    *Stack--=(unsigned int)Task>>8; // 将任务的地址高位压入堆栈，
    *Stack--=0x00; //R1 __zero_reg__
    *Stack--=0x00; //R0 __tmp_reg__
    *Stack--=0x80; //SREG 在任务中，开启全局中断
    for(i=0;i<14;i++) // 在 avr-libc 中的 FAQ 中的 What registers are used by the C compiler?
        *Stack--=i; // 描述了寄存器的作用
    TCB[TaskID].OSTaskStackTop=(unsigned int)Stack; // 将人工堆栈的栈顶，保存到堆栈的数组中
    OSRdyTbl|=OSMapTbl[TaskID]; // 将任务在就绪表中就绪
}
// 开始任务调度 , 从最低优先级的任务的开始
void OSStartTask(void)
{
    OSTaskRunningPrio=OS_TASKS;
    SP=TCB[OS_TASKS].OSTaskStackTop+17;
    __asm__ __volatile__( "reti" );
}
// 进行任务调度
void OSSched(void)
{
    // 根据中断时保存寄存器的次序入栈，模拟一次中断后，入栈的情况
    __asm__ __volatile__("PUSH __zero_reg__ "); //R1
    __asm__ __volatile__("PUSH __tmp_reg__ "); //R0
    __asm__ __volatile__("IN __tmp_reg__,__SREG__ "); // 保存状态寄存 器SREG
    __asm__ __volatile__("PUSH __tmp_reg__ ");
    __asm__ __volatile__("CLR __zero_reg__ "); //R0 重新清零
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
    __asm__ __volatile__("PUSH R28 "); //R28 与 R29 用于建立在堆栈上的指针
    __asm__ __volatile__("PUSH R29 "); // 入栈完成
    TCB[OSTaskRunningPrio].OSTaskStackTop=SP; // 将正在运行的任务的堆栈顶保存
    unsigned char OSNextTaskID; // 在现有堆栈上开设新的空间
    for (OSNextTaskID = 0; // 进行任务调度
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
void OSTimeDly(unsigned int ticks)
{
    if(ticks) // 当延时有效
    {
        OSRdyTbl &=~OSMapTbl[OSTaskRunningPrio];
        TCB[OSTaskRunningPrio].OSWaitTick=ticks;
        OSSched(); // 从新调度
    }
}

void TCN0Init(void) // 计时器 0
{
//    TCCR0 = 0;
//    TCCR0 |= (1<<CS02); // 256 预分频
//    TIMSK |= (1<<TOIE0); // T0 溢出中断允许
//    TCNT0 = 100; // 置计数起始值
//    {
        /* timer2 */
        TIMSK2 = _BV(OCIE0A);  // Enable Interrupt TimerCounter2 Compare Match A (SIG_OUTPUT_COMPARE2A)
        TCCR2A = _BV(WGM01);  // Mode = CTC
        TCCR2B = _BV(CS22);   // 8mHz / 64
        OCR2A = 55;        /* 2272 ticks per sec */
//    }
}

extern volatile  uint32_t g_jiffies;

/* timer2 */
ISR(TIMER2_COMPA_vect)
{
    ++g_jiffies; 
    if (0 == (g_jiffies % 22/*72*/)) {
        for(int i=0;i<OS_TASKS;i++) // 任务时钟
        {
            if(TCB[i].OSWaitTick)
            {
                TCB[i].OSWaitTick--;
                if(TCB[i].OSWaitTick==0) // 当任务时钟到时 , 必须是由定时器减时的才行
                {
                    OSRdyTbl |= OSMapTbl[i]; // 使任务在就绪表中置位
                }
            }
        }
    }
    
}
/*
SIGNAL(SIG_OVERFLOW0)
{
    unsigned char i;
    for(i=0;i<OS_TASKS;i++) // 任务时钟
    {
        if(TCB[i].OSWaitTick)
        {
            TCB[i].OSWaitTick--;
            if(TCB[i].OSWaitTick==0) // 当任务时钟到时 , 必须是由定时器减时的才行
            {
                OSRdyTbl |= OSMapTbl[i]; // 使任务在就绪表中置位
            }
        }
    }
    TCNT0=100;
}*/
void Task0(void)
{
    unsigned char i=0;
    while(1){
        PORTC ^= 0x08;
        OSTimeDly(50);
    }
}
void Task1(void)
{
    unsigned char i=0;
    while(1)
    {
        PORTC ^= 0x01;
        OSTimeDly(100);
    }
}
void Task2(void)
{
    unsigned char i=0;
    while(1)
    {
        OSTimeDly(200);
    }
}
void TaskScheduler(void)
{
    while(1)
    {
        OSSched(); // 反复进行调度
    }
}
int main(void)
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




