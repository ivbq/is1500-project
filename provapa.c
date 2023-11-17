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


//loop
while (true) {

    if(btns) {
        if (digitalRead(4)) { //if BT1
        }
        if (digitalRead(34)) { //if BT2
        }
        if (digitalRead(36)) { //if BT3
        }
        if (digitalRead(37)) { //if BT4
        }
    }



    IOShieldOled.clear() //rensa förra framen

    int playerHeight = 8;
    int boardWidth = 100;


    //vänster spelare
    IOShieldOled.moveTo((128-boardWidth)/2, (32-playerHeight)/2);
    IOShieldOled.drawRect(8, 8);  // int x (0-127) , int y (0-31)

    //högerspelare
    IOShieldOled.moveTo((128-boardWidth)/2 + boardWidth, (32-playerHeight)/2);
    IOShieldOled.drawRect(8, 8);  // int x , int y

    //boll
    int ballX = 64;
    int ballY = 16;
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