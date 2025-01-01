//
// Created by rocv on 25/05/22.
//

#include "delay.h"

volatile uint32_t ticks = 0;

void delay(uint32_t ms)
{
    uint32_t targetTick = ticks + ms;
    uint32_t currentTick;
    do {
        currentTick = ticks;
    } while (currentTick < targetTick);
}

uint32_t millis()
{
    return ticks;
}
