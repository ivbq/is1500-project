/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "game.h"  /* Declatations for these labs */

/* Declare a helper function which is local to this file */
static void num32asc( char * s, int ); 

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)


uint8_t screen[128][4] = {0};

uint8_t number[10][3] = {
    {0b11111000, 0b10001000, 0b11111000}, //0
    {0b10001000, 0b11111000, 0b10000000}, //1
    {0b11101000, 0b10101000, 0b10111000}, //2
    {0b10101000, 0b10101000, 0b11111000}, //3
    {0b00111000, 0b00100000, 0b11111000}, //4
    {0b10111000, 0b10101000, 0b11101000}, //5
    {0b11111000, 0b10101000, 0b11101000}, //6
    {0b00011000, 0b00001000, 0b11111000}, //7
    {0b11111000, 0b10101000, 0b11111000}, //8
    {0b10111000, 0b10101000, 0b11111000}}; //9

uint8_t text[11][3] = { //PvBTEAYHRD
    {0b11111000, 0b00101000, 0b00010000}, //P
    {0b01100000, 0b10000000, 0b01100000}, //v
    {0b11111000, 0b10101000, 0b01010000}, //B
    {0b00001000, 0b11111000, 0b00001000}, //T
    {0b11111000, 0b10101000, 0b10001000}, //E
    {0b11110000, 0b00101000, 0b11110000}, //A
    {0b11111000, 0b00100000, 0b11111000}, //H
    {0b11111000, 0b00101000, 0b11010000}, //R
    {0b11111000, 0b10001000, 0b01110000}}; //D

uint8_t pong[4][4] = { //PONG
    {0b11111000, 0b00101000, 0b00101000, 0b00010000}, //P
    {0b01110000, 0b10001000, 0b10001000, 0b01110000}, //O
    {0b11111000, 0b00010000, 0b00100000, 0b11111000}, //N
    {0b01110000, 0b10001000, 0b10101000, 0b01100000}}; //G

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

/* tick:
   Add 1 to time in memory, at location pointed to by parameter.
   Time is stored as 4 pairs of 2 NBCD-digits.
   1st pair (most significant byte) counts days.
   2nd pair counts hours.
   3rd pair counts minutes.
   4th pair (least significant byte) counts seconds.
   In most labs, only the 3rd and 4th pairs are used. */
void tick( unsigned int * timep )
{
  /* Get current value, store locally */
  register unsigned int t = * timep;
  t += 1; /* Increment local copy */
  
  /* If result was not a valid BCD-coded time, adjust now */

  if( (t & 0x0000000f) >= 0x0000000a ) t += 0x00000006;
  if( (t & 0x000000f0) >= 0x00000060 ) t += 0x000000a0;
  /* Seconds are now OK */

  if( (t & 0x00000f00) >= 0x00000a00 ) t += 0x00000600;
  if( (t & 0x0000f000) >= 0x00006000 ) t += 0x0000a000;
  /* Minutes are now OK */

  if( (t & 0x000f0000) >= 0x000a0000 ) t += 0x00060000;
  if( (t & 0x00ff0000) >= 0x00240000 ) t += 0x00dc0000;
  /* Hours are now OK */

  if( (t & 0x0f000000) >= 0x0a000000 ) t += 0x06000000;
  if( (t & 0xf0000000) >= 0xa0000000 ) t = 0;
  /* Days are now OK */

  * timep = t; /* Store new value */
}

/* display_debug
   A function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/   

/*
void display_debug( volatile int * const addr )
{
  display_string( 1, "Addr" );
  display_string( 2, "Data" );
  num32asc( &textbuffer[1][6], (int) addr );
  num32asc( &textbuffer[2][6], *addr );
  display_update();
}
*/

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}
/*
void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}
*/
void clear_screen() {
    uint8_t i;
    uint8_t j;
    for (i = 0; i < 128; i++) {
        for (j = 0; j < 4; j++) {
            screen[i][j] = 0;
        }
    }
}


void menu_update(uint8_t selected, uint8_t menu) {
    uint8_t i;
    uint8_t j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            screen[55 + i*5 + j][0] |= pong[i][j];
        }
    }
    if (menu == 1) {
        for (i = 0; i < 3; i++) {
            screen[59 + i][1] |= text[0][i]; //PvBTEAYHRD
        }
        for (i = 0; i < 3; i++) {
            screen[63 + i][1] |= text[1][i]; //PvBTEAYHRD
        }
        for (i = 0; i < 3; i++) {
            screen[67 + i][1] |= text[0][i]; //PvBTEAYHRD
        }

        for (i = 0; i < 3; i++) {
            screen[59 + i][2] |= text[2][i]; //PvBTEAYHRD
        }
        for (i = 0; i < 3; i++) {
            screen[63 + i][2] |= number[0][i]; //PvBTEAYHRD
        }
        for (i = 0; i < 3; i++) {
            screen[67 + i][2] |= text[3][i]; //PvBTEAYHRD
        }
    } else { //menu 2 bot diff
        for (i = 0; i < 3; i++) {
            screen[58 + i][1] |= text[0][i]; //PvBTEAYHRD
        }
        for (i = 0; i < 3; i++) {
            screen[62 + i][1] |= text[1][i]; //PvBTEAYHRD
        }
        for (i = 0; i < 3; i++) {
            screen[66 + i][1] |= text[0][i]; //PvBTEAYHRD
        }

        for (i = 0; i < 3; i++) {
            screen[58 + i][2] |= text[2][i]; //PvBTEAYHRD
        }
        for (i = 0; i < 3; i++) {
            screen[62 + i][2] |= number[0][i]; //PvBTEAYHRD
        }
        for (i = 0; i < 3; i++) {
            screen[66 + i][2] |= text[3][i]; //PvBTEAYHRD
        }
    }

    if (selected == 0) {
        for (i = 0; i < 28; i++) {
            screen[50+i][1] |= 0b00000010;
            screen[50+i][2] |= 0b00000010;
        }
        screen[50][1] |= 0b11111110;
        screen[50][2] |= 0b00000011;
        screen[50+28][1] |= 0b11111110;
        screen[50+28][2] |= 0b00000011;
    } else {
        for (i = 0; i < 28; i++) {
            screen[50+i][2] |= 0b00000010;
            screen[50+i][3] |= 0b00000010;
        }
        screen[50][2] |= 0b11111110;
        screen[50][3] |= 0b00000011;
        screen[50+28][2] |= 0b11111110;
        screen[50+28][3] |= 0b00000011;
    }
}


void update_ball(uint8_t x, uint8_t y) {
    uint8_t j;
    uint8_t i;
    y --;
    
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            if (y%8 <= 5) {
                screen[x+j-1][y/8] |= 0b111 << y%8;
            } else {
                int combined = 0b111 << y%8;
                screen[x+j-1][y/8] |= (uint8_t)(combined & 0xff);
                screen[x+j-1][y/8 + 1] |= (uint8_t)((combined & 0xff00) >> 8);
            }
        }
    }
}

void update_paddle(uint8_t x, uint8_t y) {
    uint8_t i, j, k;
    for (j = 0; j < 2; j++) {
        uint32_t combined = 0b1111111111 << y;

        for (i = 0; i < 4; i++) {
            screen[x-j][i] |= (uint8_t)(combined & 0xff);
            combined = combined >> 8;
        }
    }
}

void update_score(uint8_t left, uint8_t right){
    uint8_t i;
    for(i = 0; i < 128; i++) {
        screen[i][0] |= 0x1;
        screen[i][3] |= 0x80;
    }
    for (i = 0; i < 3; i++) {
        screen[55 + i][0] |= number[left][i]; //xor?
        screen[68 + i][0] |= number[right][i];
    }
}

void display_update(void) {
    uint8_t i, j, k;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
        for(j = 0; j < 128; j++) {
            spi_send_recv(screen[j][i]);
        }
	}
}



/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n ) 
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}


/*
 * itoa
 * 
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 * 
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 * 
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 * 
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 * 
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 * 
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 * 
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 * 
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";
  
  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}