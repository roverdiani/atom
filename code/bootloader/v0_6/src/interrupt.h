//
// Created by rocv on 23/05/22.
//

#ifndef INC_68K_SRC_INTERRUPT_H
#define INC_68K_SRC_INTERRUPT_H

#include <stdio.h>

typedef void(*intvector)(void);

void intSpurious();
void int2();
void int5();
void int7();

void enable_timer_int();

__attribute__((unused)) static intvector intTable[8] =
        {
                intSpurious,
                NULL,
                int2,
                NULL,
                NULL,
                int5,
                NULL,
                int7
        };

#endif //INC_68K_SRC_INTERRUPT_H
