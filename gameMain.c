#include <stdio.h>
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <stdlib.h>
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "game.h"  /* Declatations for these labs */
#include <math.h>
#include <errno.h>

#define EEPROM_ADDR 0x50 // Address of the EEPROM

/*
 * TODO:    Ball movement
 *          ball collision (även y-led)
 *          highscore
 *          vinn anouncement, meny eller separat sida?
*/

struct ball ball;
void init_ball() {
    ball.x = 64;
    ball.y = 16;
    ball.vx = 0.7;
    ball.vy = 0.7;
};

struct paddle left;
struct paddle right;
void init_paddles() {
    right.y = 10;
    right.score = 0;
    left.y = 10;
    left.score = 0;
};

uint8_t playerHeight = 10;
uint8_t widthMargin = 10;
uint8_t heightMargin = 1;

struct bot bot;
void init_bot() {
    bot.dir = 1;
    bot.diff = 1; //0 easy, 1 hard
};

// Only 3 highscores are visible at a time, though 10 are kept in memory
// Highscore format:
// 0-2 reserved for 3-letter name ABC
// 3 is a separator
// 4-8 lists total points scored
typedef struct highscore {
    char name[3];
    int score;
} Highscore;

Highscore highscores[10];

int cmpfunc(const void *a, const void *b) {
    const Highscore *highscoreA = (const Highscore *)a;
    const Highscore *highscoreB = (const Highscore *)b;
    return highscoreA->score - highscoreB->score;
}

void ball_collision() { //fixa studs på y ???
    if (ball.y <= heightMargin+1) { //if tak
        ball.y = heightMargin+1;
        ball.vy *= -1;
    }
    if(ball.y >= 31 - heightMargin -1) { //if golv
        ball.y = 31 - heightMargin -1;
        ball.vy *= -1;
    }

    float magnitude = 1;
    if (ball.x < widthMargin+2 && ball.x >= widthMargin) { //if vid vänstra spelare
        if (left.y-1 < ball.y && left.y + playerHeight +1 > ball.y) {
            ball.x = widthMargin+2;
            ball.vx = 2;
            ball.vy = ball.y - (left.y + (playerHeight/2));
            magnitude = (ball.vx * ball.vx + ball.vy * ball.vy);
            ball.vx *= ball.vx / magnitude;
            ball.vy *= ball.vy / magnitude;
            if (ball.y < (left.y + (playerHeight/2))) {
                ball.vy *= -1;
            }
        }
    } else if (ball.x > 127 - widthMargin -2 && ball.x <= 127 - widthMargin) { //if vid högra spelare
        if (right.y-1 < ball.y && right.y + playerHeight +1 > ball.y) {
            ball.x = 127 - widthMargin -2;
            ball.vx = 2;
            ball.vy = ball.y - (right.y + (playerHeight/2));
            magnitude = (ball.vx * ball.vx + ball.vy * ball.vy);
            ball.vx *= -ball.vx / magnitude;
            ball.vy *= ball.vy / magnitude;
            if (ball.y < (right.y + (playerHeight/2))) {
                ball.vy *= -1;
            }
        }
    }

    bool outl = ball.x <= 2;
    bool outr = ball.x >= 125;
    
    if (outl || outr) {
        ball.x = 64, ball.y = 16;
        ball.vx = 0.7, ball.vy = 0.7;
        if (outl) {right.score++;} else {left.score++;}

        bool is_pve = false;
        if (is_pve) {
            char *winner = (right.score > left.score) ? "rrr" : "lll";
            int i;
            for (i = 0; i < 10; i++) {
                if (highscores[i].name == winner) {
                    highscores[i].score++;
                    break;
                }
            }
            qsort(highscores, 10, sizeof(Highscore), cmpfunc);
        }
    }
}

void bot_movement() {
    if (bot.diff) {
        if (ball.y-3  < right.y) {
            right.y--;
            if (right.y < heightMargin) {
                right.y = heightMargin;
            }
        } else if (ball.y+4 > right.y + playerHeight) {
            right.y++;
            if (right.y > 31-playerHeight) {
                right.y = 31-playerHeight;
            }
        }
    } else {
        right.y += bot.dir;
        if (right.y < heightMargin) {
            right.y = heightMargin;
            bot.dir *= -1;
        } else if (right.y > 31-playerHeight - heightMargin) {
            right.y = 31-playerHeight - heightMargin;
            bot.dir *= -1;
        }
    }
}


int main(void) {
    /*
	  This will set the peripheral bus clock to the same frequency
	  as the sysclock. That means 80 MHz, when the microcontroller
	  is running at 80 MHz. Changed 2017, as recommended by Axel.
	*/
	SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
	while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
	SYSKEY = 0x0;  /* Lock OSCCON */
	
	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
    SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;

    /* I2C device address of EEPROM chip */
    int address = 1010000;

    /* Temporary variable */
    uint16_t temp;

    /* Set up i2c */
	I2C1CON = 0x0;
	/* I2C Baud rate should be less than 400 kHz, is generated by dividing
	the 40 MHz peripheral bus clock down */
	I2C1BRG = 0x0C2;
	I2C1STAT = 0x0;
	I2C1CONSET = 1 << 13; //SIDL = 1
	I2C1CONSET = 1 << 15; // ON = 1
	temp = I2C1RCV; //Clear receive buffer
	
    display_init();
    init_ball();
    init_paddles();
    init_bot();
    int delay;
    uint8_t pvp = 0;
    uint8_t menu = 1; //0 är in game, 1 är main meny, 2 är play meny, 3 är bot difficulty select, 4 score //5 vinnmeny???
    uint8_t selected = 0; //selected button i meny

    while(1==1){
        //buttons
        uint8_t btns = getbtns();
        if(btns) {
            if ((btns & 0b100) == 0b100) { //if BT3
                if (menu) {
                    selected = 1;
                } else {
                    left.y++;
                    if (left.y > 32-playerHeight - heightMargin) {
                        left.y = 32-playerHeight - heightMargin;
                    }
                }
            }
            if ((btns & 0b1000) == 0b1000) { //if BT4
                if (menu) {
                    selected = 0;
                } else {
                    left.y--;
                    if (left.y < heightMargin) {
                        left.y = heightMargin;
                    }
                }
            }
            if ((btns & 0b10) == 0b10) { //if BT2
                if (menu) { //menyknapp
                    if (menu == 1) {
                        if (selected == 0) { //play
                            menu = 2;
                        } else { //score
                            menu = 4;
                        }
                    } else if (menu == 2){ //menu 2
                        if (selected == 0) { //pvp
                            pvp = 1;
                            menu = 0;
                        } else { //bot
                            menu = 3;
                        }
                    } else if (menu == 3){ //menu 3
                        if (selected == 0) { //easy bot
                            bot.diff = 0;
                        } else { //hard bor
                            bot.diff = 1;
                        }
                        pvp = 0;
                        menu = 0;
                    } else { //menu 4 score
                    // Add logic to handle highscores
                        menu = 1;
                        selected = 0;
                    }

                } else { //movement knapp
                    if (pvp) {
                        right.y++;
                        if (right.y > 31-playerHeight) {
                            right.y = 31-playerHeight;
                        }
                    }
                }
            }


            if ((btns & 0b1) == 0b1) { //if BT1
                if (pvp && !menu) {
                    right.y--;
                    if (right.y < heightMargin) {
                        right.y = heightMargin;
                    }
                }
            }
        }



        clear_screen();
        if (menu) {
            menu_update(selected, menu);
        } else {
            ball.y += ball.vy;
            ball.x += ball.vx;
            ball_collision(); //studsar/vinn
            
            if (!pvp) { bot_movement(); } //om man spelar mot en bot
            update_paddle(widthMargin,left.y);
            update_paddle(128-widthMargin,right.y);
            update_ball((uint8_t)ball.x, (uint8_t)ball.y);
            if (right.score == 10 || left.score == 10) {
                left.score = 0;
                right.score = 0;
                menu = 1;
                clear_screen();
            } else {
                update_score(left.score, right.score);
            }
        }
        display_update();

        if ((btns & 0b10) == 0b10 && menu) {
            for(delay = 0; delay < 2000000; delay++) {}
        }

        
        if (menu) {
            delay = 10000;
        } else {
            delay = 100000;
        }
        for(delay; delay > 0; delay--) {}
    }
}