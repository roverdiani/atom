#include "terminal.h"
#include "defines.h"
#include <stdio.h>

void terminal_clear()
{
    printf("%c[2J%c[H", ASCII_ESC, ASCII_ESC);
}
