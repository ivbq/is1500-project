# Advanced project: Pong

## Objective

This project involves designing and implementing a Pong game on a chipKIT-board. To reach the "advanced" project-level, we will keep track of highscores (games won) with non-volatile EEPROM memory as to keep scores saved when shut off. It will feature a 2-player mode alongside a 1-player mode against a CPU opponent with two difficulty modes.

## Requirements

### Game logic: 
Implement basic game logic to handle paddles and the ball.

### Player modes: 
Implement handling for multiplayer and versus CPU mode with multiple difficulties.

### Display interface: 
Implement rendering the game state alongside menu options for changing player modes, difficulty, and viewing highscores.

### High-score tracking: 
Implement tracking highscores (games won) and viewing them in a simple menu.

### Control interface: 
Implement controls using buttons and switches on the chipKIT.

## Implementation details

This project will mainly be written in C, with limited use of MIPS assembly code. Markdown and plain text files may be used for documentation and specification purposes.

### High-score tracking:

J3: Analog Signal Connector
Pin 1 (analog signal A0) is connected to a potentiometer. Pins 5 and 6 (labeled A4 and A5) provide
access to the I 2 C signals SDA and SCL for the I2C bus. The I2C bus is used to access the temperature
sensor and the EEPROM and for the I2C daisy chain connector.

EEPROM: A 256Kbit (32Kbyte), I2C EEPROM is provided using a Microchip 24LC256. This EEPROM, IC1, is located on the bottom left of the shield, just below the chipKIT logo. The 7-bit I2C device address for the EEPROM is ‘1010000’.

Digilent has a library for using the EEPROM. It is contained in document # DSD-0000311 (chipKIT IOShield
Library.zip) which can be downloaded from the Basic I/O Shield product page at www.digilentinc.com. The
EEPROM library is IOShieldEEPROM.