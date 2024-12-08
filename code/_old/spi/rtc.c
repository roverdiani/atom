//
// Created by rocv on 24/05/22.
//

#include "rtc.h"
#include "defines.h"

void rtc_init(struct rtc *self, uintptr_t address)
{
    self->address = address;

    // 00:00:00
    PORT_IO(self->address + 0x00) = 0x00;
    PORT_IO(self->address + 0x01) = 0x00;
    PORT_IO(self->address + 0x02) = 0x00;
    PORT_IO(self->address + 0x03) = 0x00;
    PORT_IO(self->address + 0x04) = 0x00;
    PORT_IO(self->address + 0x05) = 0x00;

    // Thursday, 01/01/1970
    PORT_IO(self->address + 0x06) = 0x01;
    PORT_IO(self->address + 0x07) = 0x00;
    PORT_IO(self->address + 0x08) = 0x01;
    PORT_IO(self->address + 0x09) = 0x00;
    PORT_IO(self->address + 0x0A) = 0x00;
    PORT_IO(self->address + 0x0B) = 0x07;
    PORT_IO(self->address + 0x0C) = 0x04;

    // Clear flags
    PORT_IO(self->address + 0x0D) = 0x00;

    // 1 second, interrupt mode, masked
    PORT_IO(self->address + 0x0E) = 0x07;

    // 24 hour mode
    PORT_IO(self->address + 0x0F) = 0x04;
}

void rtc_enable_interrupt(struct rtc *self)
{
    // 1 second, interrupt mode, not masked
    PORT_IO(self->address + 0x0E) = 0x06;
}
