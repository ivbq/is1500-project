# Pong (advanced project)

## Installation

1. Install mcb32tools from the Github respository: https://github.com/is1200-example-projects/mcb32tools/releases/
2. Connect your chipkit to your device by cable
3. Determine the name of the USB serial port it's on, for example, `/dev/ttyUSB0` on Linux
4. Go into your mcb32 environment using `. opt/mcb32tools/environment` 
5. Compile the code by going into the correct folder and use the command: `make`
6. Install the code with the "make install" command, replacing the path with the one you identified: `make install TTYDEV=/dev/tty_dev_identified_above`

## Controls

Button 4 is up (selection and movement for player 1)
Button 3 is down (selection and movement for player 1)
Button 2 is continue in menu and for up-movement for player 2 in PvP mode
Button 1 is "back to main menu" in menu and for down-movement for player 2 in PvP mode
