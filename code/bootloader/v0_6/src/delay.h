//
// Created by rocv on 25/05/22.
//

#ifndef INC_68K_SRC_DELAY_H
#define INC_68K_SRC_DELAY_H

#include "interrupt.h"
#include <stdint.h>

void delay(uint32_t ms);
uint32_t millis();

#endif //INC_68K_SRC_DELAY_H
