//
// Created by rocv on 24/05/22.
//

#ifndef INC_68K_SRC_RTC_H
#define INC_68K_SRC_RTC_H

#include "stdint.h"

struct rtc {
    uintptr_t address;
};

void rtc_init(struct rtc *self, uintptr_t address);
void rtc_enable_interrupt(struct rtc *self);

#endif //INC_68K_SRC_RTC_H
