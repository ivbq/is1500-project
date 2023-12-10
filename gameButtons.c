#include <stdint.h>
#include <pic32mx.h>
#include "game.h" 

uint8_t getsw(void) { //return 4 bits being SW4, SW3, SW2 and SW1
    volatile int* portd = (volatile int*) 0xbf8860D0;
    return ((uint8_t)(*portd & 0xF00) >> 8);
}

uint8_t getbtns(void) { //return 4 bits being BT4, BT3 and BT2, BT1
    volatile int* portd = (volatile int*) 0xbf8860D0; //3 bits being BT4, BT3 and BT2
    return ((uint8_t)((*portd & 0xE0) >> 4) | (PORTF >> 1) & 0x1);
}