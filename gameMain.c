#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "game.h"  /* Declatations for these labs */


struct ball ball;
void init_ball() {
    ball.x = 64;
    ball.y = 16;
    ball.vx = 1;
    ball.vy = 1;
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


void ball_collision() {
    if (ball.y < heightMargin+2 || ball.y > 31 - heightMargin -2) { //if tak/golv
        ball.vy *= -1;
    }
    if (ball.x == widthMargin+2) { //if vid vänstra spelare
        if (left.y-1 < ball.y && left.y + playerHeight +1 > ball.y) {
            ball.vx *= -1;
        }
    } else if (ball.x == 127 - widthMargin -2) { //if vid högra spelare
        if (right.y-1 < ball.y && right.y + playerHeight +1 > ball.y) {
            ball.vx *= -1;
        }
    }

    //någon förlorar
    if (ball.x <= 1) { //vänster ut
        ball.x = 64;
        ball.vx *= -1;
        right.score ++;
    } else if (ball.x >= 126) { //höger ut
        ball.x = 64;
        ball.vx *= -1;
        left.score++;
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
	
    display_init();
    init_ball();
    init_paddles();
    init_bot();
    int delay;
    uint8_t pvp = 0;
    uint8_t menu = 1; //0 är in game, 1 är main meny, 2 är play meny, 3 är bot difficulty select, 4 score
    uint8_t btnsFlag = 0;
    
    uint8_t selected = 0; //selected button i meny

    

    while(1==1){
        //buttons
        uint8_t btns = getbtns();
        if(btns) {
            if ((btns & 0b1000) == 0b1000) { //if BT4
                if (menu) {
                    selected = 1;
                } else {
                    left.y++;
                    if (left.y > 32-playerHeight - heightMargin) {
                        left.y = 32-playerHeight - heightMargin;
                    }
                }
            }
            if ((btns & 0b100) == 0b100) { //if BT3
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
            //ball movement
            ball.y += ball.vy;
            ball.x += ball.vx;
            if (!pvp) { bot_movement(); } //om man spelar mot en bot
            update_paddle(widthMargin,left.y);
            update_paddle(128-widthMargin,right.y);
            update_ball(ball.x, ball.y);
            ball_collision(); //studsar/vinn
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
            delay = 200000;
        }
        for(delay; delay > 0; delay--) {}
    }
}