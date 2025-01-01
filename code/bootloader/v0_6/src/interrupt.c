#include "interrupt.h"
#include "defines.h"

volatile unsigned int ticks;

__attribute__((interrupt)) void intSpurious()
{
}

__attribute__((interrupt)) void int2()
{
    ticks++;
    PORT_IO(ADDRESS_IO_BOARD_INT_CLEAR) = 0x00;
}

__attribute__((interrupt)) void int5()
{
}

__attribute__((interrupt)) void int7()
{
}

void enable_timer_int()
{
    PORT_IO(ADDRESS_IO_BOARD_INT_ENABLE) = 0x00;
}
