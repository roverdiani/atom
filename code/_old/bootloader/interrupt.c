#include "interrupt.h"
#include "defines.h"

volatile unsigned int ticks;

__attribute__((interrupt)) void intSpurious()
{
}

__attribute__((interrupt)) void int2()
{
    ticks++;
    PORT_IO(INT_ADDR) = 0x00;
}

__attribute__((interrupt)) void int5()
{
}

__attribute__((interrupt)) void int7()
{
}

void enable_timer_int()
{
    PORT_IO(INT_E_ADDR) = 0x00;
}
