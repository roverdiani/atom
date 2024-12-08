*-----------------------------------------------------------
* Title      : 68k Monitor
* Written by : rocv
* Date       : 19/04/2022
* Description: A simple monitor
*-----------------------------------------------------------
    ORG    $F0000
    
FT245   EQU $000E0000
STACK_B EQU $00010000

    dc.l    STACK_B
    dc.l    START

START:
    nop
    jmp     START

    SIMHALT

* Put variables and constants here

    END    START







*~Font name~Courier New~
*~Font size~10~
*~Tab type~1~
*~Tab size~4~
