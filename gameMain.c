#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <stdlib.h>
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "game.h"  /* Declatations for these labs */

#define EEPROM_ADDR 0x50 // Address of the EEPROM

/*
 * TODO:    
 *      highscore???
 *      fix game.h
 *      readme.txt
 *      comments
*/

// Only 3 highscores are visible at a time, though 10 are kept in memory
// Highscore format:
// 0-2 reserved for 3-letter name ABC
// 3 is a separator
// 4-8 lists total points scored
typedef struct highscore {
    char name[4];
    uint16_t score;
} Highscore;

Highscore highscores[8];

struct ball ball;
//initialize ball
void init_ball() {
    ball.x = 64;
    ball.y = 16;
    ball.vx = 0.7;
    ball.vy = 0.7;
};

struct paddle left;
struct paddle right;
//initialize paddles
void init_paddles() {
    right.y = 10;
    right.score = 0;
    left.y = 10;
    left.score = 0;
};

//game constants
const uint8_t playerHeight = 10;
const uint8_t widthMargin = 10;
const uint8_t heightMargin = 1;

struct bot bot;
//initialize bot
void init_bot() {
    bot.dir = 1;
    bot.diff = 1; //0 easy, 1 hard
};

int cmpfunc(const void *a, const void *b) {
    const Highscore *highscoreA = (const Highscore *)a;
    const Highscore *highscoreB = (const Highscore *)b;
    return highscoreB->score - highscoreA->score ;
}

//checks if the ball collides with roof/floor or any player or if any player scored
void ball_collision() {
    if (ball.y <= heightMargin+1) { //if roof
        ball.y = heightMargin+1;
        ball.vy *= -1;
    }
    if(ball.y >= 31 - heightMargin -1) { //if floor
        ball.y = 31 - heightMargin -1;
        ball.vy *= -1;
    }

    float magnitude = 1;
    float newBallVy = 0;
    float newBallVx = 0;
    if (ball.x < widthMargin+2 && ball.x >= widthMargin) { //if by the left player
        if (left.y-1 < ball.y && left.y + playerHeight +1 > ball.y) { //if collide
            ball.x = widthMargin+2;

            //new vector
            newBallVx = 2;
            newBallVy = ball.y - (left.y + (playerHeight/2));
            magnitude = (newBallVx*newBallVx + newBallVy*newBallVy);
            newBallVx *= newBallVx / magnitude;
            newBallVy *= newBallVy / magnitude;
            if (ball.y < (left.y + (playerHeight/2))) {
                newBallVy *= -1;
            }
            //add new vectors
            ball.vx *= -1;
            ball.vx += newBallVx;
            ball.vy += newBallVy;
            //normalize resulting vector
            magnitude = (ball.vx * ball.vx + ball.vy * ball.vy);
            ball.vx *= ball.vx / magnitude;
            if (ball.vy > 0) {
                ball.vy *= ball.vy / magnitude;
            } else {
                ball.vy *= -ball.vy / magnitude;
            }
        }
    } else if (ball.x > 127 - widthMargin -2 && ball.x <= 127 - widthMargin) { //if by right player
        if (right.y-1 < ball.y && right.y + playerHeight +1 > ball.y) { //if collide
            ball.x = 127 - widthMargin -2;
            
            //new vector
            newBallVx = 2;
            newBallVy = ball.y - (right.y + (playerHeight/2));
            magnitude = (newBallVx*newBallVx + newBallVy*newBallVy);
            newBallVx *= newBallVx / magnitude;
            newBallVy *= newBallVy / magnitude;
            if (ball.y < (right.y + (playerHeight/2))) {
                newBallVy *= -1;
            }

            //add new vector
            ball.vx += newBallVx;
            ball.vy += newBallVy;
            magnitude = (ball.vx * ball.vx + ball.vy * ball.vy);
            //normalize resulting vector
            ball.vx *= ball.vx / magnitude;
            if (ball.vy > 0) {
                ball.vy *= ball.vy / magnitude;
            } else {
                ball.vy *= -ball.vy / magnitude;
            }
            //the ball needs to go left (ball.vx needs to be negative)
            ball.vx *= -1;
        }
    }

    // Check if player has scored goal
    bool outl = ball.x <= 2;
    bool outr = ball.x >= 125;
    if (outl || outr) {
        // Reset ball position
        ball.x = 64, ball.y = 16;
        ball.vx = 0.7, ball.vy = 0.7;

        // Add to in-game score counter
        if (outl) {right.score++;} else {left.score++;}
    }
}

void bot_movement() {
    //if bot is hard mode
    if (bot.diff) {
        //if the ball is nearly above the paddle
        if (ball.y-3  < right.y) {
            right.y--; //move up
            if (right.y < heightMargin) {
                right.y = heightMargin;
            }
        } else if (ball.y+4 > right.y + playerHeight) { //if the ball is nearly below the paddle
            right.y++; //move down
            if (right.y > 31-playerHeight) {
                right.y = 31-playerHeight;
            }
        }
    } else { //if bot is easy mode
        right.y += bot.dir;
        if (right.y < heightMargin) { //if it hits the roof, change direction
            right.y = heightMargin;
            bot.dir *= -1;
        } else if (right.y > 31-playerHeight - heightMargin) { //if it hits the floor, change direction
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
	
    //initialize everything
    display_init();
    init_ball();
    init_paddles();
    init_bot();
    uint8_t pvp = 0;
    uint8_t menu = 1; //0 är in game, 1 är main meny, 2 är play meny, 3 är bot difficulty select, 4 name select, 5 score, 6 väntervinnmeny, 7 högervinnmeny
    uint8_t selected = 0; //selected button i meny
    char name[4] = "AAA";
    uint8_t selected_char = 0;
    //controls roughly how long each frame is
    int delay;

    // Populate highscore table
    int i;
    for (i = 0; i < 8; i++) {
        highscores[i] = (Highscore) {"   ", 0};
    }

    while(1==1){
        //buttons
        uint8_t btns = getbtns();
        if(btns) {
            if ((btns & 0b1000) == 0b1000) { //if BT4
                if (menu == 4) {
                    if (name[selected_char] > 'A') {
                        name[selected_char]--;
                    } else {
                        name[selected_char] = 'Z';
                    }
                }
                else if (menu) {
                    selected = 0;
                } else {
                    left.y--;
                    if (left.y < heightMargin) {
                        left.y = heightMargin;
                    }
                }
            }
            if ((btns & 0b100) == 0b100) { //if BT3
                if (menu == 4) {
                    if (name[selected_char] < 'Z') {
                        name[selected_char]++;
                    } else {
                        name[selected_char] = 'A';
                    } 
                }
                else if (menu) {
                    selected = 1;
                } else {
                    left.y++;
                    if (left.y > 32-playerHeight - heightMargin) {
                        left.y = 32-playerHeight - heightMargin;
                    }
                }
            }
            if ((btns & 0b10) == 0b10) { //if BT2
                if (menu) { //menyknapp
                    if (menu == 1) {
                        if (selected == 0) { //play
                            menu = 2;
                        } else { //score
                            menu = 5;
                            selected = 0;
                        }
                    } else if (menu == 2){ //menu 2
                        if (selected == 0) { //pvp
                            pvp = 1;
                            menu = 0;
                        } else { //bot
                            menu = 3;
                            selected = 0;
                        }
                    } else if (menu == 3){ //menu 3
                        if (selected == 0) { //easy bot
                            bot.diff = 0;
                        } else { //hard bot
                            bot.diff = 1;
                        }
                        pvp = 0;
                        menu = 4;
                        selected = 0;
                    } else if (menu == 4) { // Name select
                        if (selected_char < 2) {
                            selected_char++;
                        } else {
                            right.y = 5; //för annars missar den bollen
                            menu = 0;
                            selected_char = 0;
                        }
                    } else if (menu == 5) { //menu 5 score
                        menu = 1;
                        selected = 0;
                    } else if (menu == 6 || menu == 7) { //win screen
                        strcpy(name, "AAA");
                        menu = 1;
                        selected = 0;
                    } else { //default
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
                if (menu) {
                    menu = 1;
                }
                else if (pvp && !menu) {
                    right.y--;
                    if (right.y < heightMargin) {
                        right.y = heightMargin;
                    }
                }
            }
        }

        clear_screen();
        if (menu) {
            menu_update(selected, menu, highscores, name, selected_char);
        } else {
            ball.y += ball.vy;
            ball.x += ball.vx;
            ball_collision(); //studsar/vinn
            
            if (!pvp) { bot_movement(); } //om man spelar mot en bot

            update_paddle(widthMargin,left.y);
            update_paddle(128-widthMargin,right.y);
            update_ball((uint8_t)ball.x, (uint8_t)ball.y);
            
            if (left.score == 1 || right.score == 1) {
                if (left.score > right.score) {
                    if (!pvp) {
                        int i;
                        for (i = 0; i < 8; i++) {
                            if (highscores[i].score == 0) {
                                strcpy(highscores[i].name, name);
                                highscores[i].score = 1;
                                break;
                            } else if (!strcmp(highscores[i].name, name)) {
                                highscores[i].score++;
                                break;
                            }
                        }
                    }
                    qsort(highscores, 8, sizeof(Highscore), cmpfunc);
                    menu = 6;
                } else {
                    menu = 7;
                }

                left.score = 0;
                right.score = 0;
                left.y = 10;
                right.y = 10;
                ball.x = 64;
                ball.y = 16;
                ball.vx = 0.7;
                ball.vy = 0.7;
            } else {
                update_score(left.score, right.score);
            }
        }
        display_update();

        if ((btns & 0b10) == 0b10 && menu) {
            delay = 1500000;
        } else if (menu == 4 && btns) {
            delay = 800000;
        } else if (menu && btns) {
            delay = 200000;
        } else if (menu == 0){
            delay = 100000;
        }
        for(delay; delay > 0; delay--) {}
    }
}