//
// Created by rocv on 25/05/22.
//

#include "delay.h"

volatile unsigned int ticks = 0;

void delay(int ms)
{
    unsigned int targetTick = ticks + ms;
    unsigned int currentTick;
    do {
        currentTick = ticks;
    } while (currentTick < targetTick);
}

unsigned int millis()
{
    return ticks;
}
