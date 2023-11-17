#include <stdint.h>
#include <pic32mx.h>
#include <IOShieldOled.h>
//#include "mipslab.h" 

byte* fillWhite;

//setup
IOShieldOled.begin(); //ström
IOShieldOled.displayOn() //sätt på
IOShieldOled.clear() //clear buffer
IOShieldOled.setDrawMode(modeSet)
 
fillWhite = IOShieldOled.getStdPattern(1);
IOShieldOled.setFillPattern(fillWhite);

pinMode(4, INPUT); //BTN1
pinMode(34, INPUT); //BTN2
pinMode(36, INPUT); //BTN3
pinMode(37, INPUT); //BTN4

int playerHeight = 8;
int boardWidth = 100;

int player1pos = 15;
int player2pos = 15;
int ballX = 64;
int ballY = 16;
int ballDir[] = {1,5};
int currBallDir[] = {0,0};


//loop
while (true) {

    //update player position
    if(btns) {
        if (digitalRead(4)) { //if BT1 //ner
            player1pos += 1;
            if (player1pos == 32 - (playerHeight/2)) {player1pos = 31 - (playerHeight/2);}
        }
        if (digitalRead(34)) { //if BT2 //upp
            player1pos -= 1;
            if (player1pos == -1 + (playerHeight/2)) {player1pos = 0 + (playerHeight/2);}
        }
        if (digitalRead(36)) { //if BT3
            player2pos += 1;
            if (player2pos == 32 - (playerHeight/2)) {player2pos = 31 - (playerHeight/2);}
        }
        if (digitalRead(37)) { //if BT4
            player2pos -= 1;
            if (player2pos == -1 + (playerHeight/2)) {player2pos = 0 + (playerHeight/2);}
        }
    }

    //update ball position

    //move ball
    if (currBallDir[0] == ballDir[0] && currBallDir[1] == ballDir[1]) {
        currBallDir[0] = 0;
        currBallDir[1] = 0;
    }

    if (ballDir[0] > currBallDir[0] && balldir[0] > 0) {
        currBallDir[0] ++;
        ballX ++;
    } else if (ballDir[0] < currBallDir[0] && balldir[0] < 0) {
        currBallDir[0] --;
        ballX --;
    } else if (ballDir[1] > currBallDir[1] && balldir[1] > 0) {
        currBallDir[1] ++;
        ballY ++;
    } else if (ballDir[1] < currBallDir[1] && balldir[1] < 0) {
        currBallDir[1] --;
        ballY --;
    }

    //ball bounce on wall

    if(ballY == -1) {
        ballY = 1;
        ballDir[1] *= -1;
        currBallDir[1] = 0;
    } else if(ballY == 32) {
        ballY = 30;
        ballDir[1] *= -1;
        currBallDir[1] = 0;
    }


    //ball bounce on player






    /////////////////////////////////////
    //////////// Oled grafik ////////////
    /////////////////////////////////////

    IOShieldOled.clear() //rensa förra framen
    IOShieldOled.moveTo(0,0);
    IOShieldOled.drawRect(127, 0);
    IOShieldOled.moveTo(0,31);
    IOShieldOled.drawRect(127, 31);


    //vänster spelare
    IOShieldOled.moveTo(    (128-boardWidth)/2 -1,              player1pos - (playerHeight/2));
    IOShieldOled.drawRect(  (128-boardWidth)/2,                 player1pos + (playerHeight/2));  // int x (0-127) , int y (0-31)

    //högerspelare
    IOShieldOled.moveTo(    (128-boardWidth)/2 + boardWidth,    player2pos - (playerHeight/2));
    IOShieldOled.drawRect(  (128-boardWidth)/2 + boardWidth +1, player2pos + (playerHeight/2));  // int x , int y

    //boll

    IOShieldOled.moveTo(ballX-1, ballY-1);
    IOShieldOled.drawRect(ballX+1, ballY+1);  // int x , int y


    IOShieldOled.updateDisplay();
    delay(1000);
}

void delay(int ms) {
    for(int i = 0; i < ms; i++) {
        for(int j = 0; j < 4711; j++) {}
    }
}