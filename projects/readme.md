# AVR Libraries

Contains my 6502 libraries ported to AVR

## Libraries

### timer
Implements my timer library from 6502 for an AVR by using an AVR timer to implement the functions with exactly the same interface.

### spi   
Implements my SPI library from 6502 for an AVR by using the AVR master SPI to implement the functions with exactly the same interface.

### sdcard
The exact code used on the 6502. It depends only on the SPI and timer libraries.

### fatfs
The standard FatFS library without any modification. `diskio.c` is adapted to use my sdcard library.

---

## Templates

The templates are 4 projects to help build a custom project.

As many people need to use a serial port in AVR projects and as I want to keep these projects simple, all these projects implement an echo on TX of what is received on RX of the serial port 0 (if several).

### avr-tiny-asm
As flash/RAM of the AVR Tiny are very limited, I've chosen to create a project using **avra**.  
**avra** is a free AVR assembler very close to the original avrasm from Atmel, so old code should be easy to port with this template.  
On this project the serial port is emulated and the speed limited.  
Note that by lack of hardware this project is not fully tested (it will be soon).

### avr-mega-asm
This is roughly the same idea as for the AVR Tiny but this time uses the real serial port 0.  
It was fully tested on Arduino Uno, Arduino Mega 2560, and on a proto board with an ATmega1284.  
The ATmega1284 is a nice chip with 128K flash, 16K RAM, and supports debugging via JTAG.  
Choose this template if you want to recover old AVR code for AVR ATmega.

### avr-mega-c
Same thing as avr-mega-asm except that this time the code is in pure C (GCC), not the awful Arduino code.  
It was fully tested on Arduino Uno, Arduino Mega 2560, and on a proto board with an ATmega1284.  
Choose this template if you want to build a pure C project.

### avr-mega-c-asm
Same as above but the goal of this template is to show how to mix C (GCC C) and assembler.  
Note that **avra** cannot be used. This time the assembler program linked to the C must be assembled by the GCC assembler.  
The syntax is different from the standard AVR assembler - check carefully the differences to avoid problems.  
There is a small difference in this project: it still echoes the data received but this time in hexadecimal.  
The conversion is done by the assembly program.  
It was fully tested on Arduino Uno, Arduino Mega 2560, and on a proto board with an ATmega1284.  
Choose this template if you want to build a mixed C/Assembly project.

---

## Tests

These tests were written to test some functions on the AVR / SPI / timer.

### blink
Just blinks the LED of an Arduino / proto board.  
This program was also written to verify that the timer library was working.  
The timer library is used for the delay between 2 LED state changes.

### spi-test
Small test to verify that SPI worked on the Arduino Mega.  
If you read the Arduino documentation you have the feeling that the Arduino shields are compatible between the Uno and Mega, and in fact it is not the case.  
Apparently pin N arrives on pin N but they are not mapped the same way.  
On D10/11/12/13 you have SPI on the Uno but it's not the case on the Mega, so if your SD card shield is not taking SPI on the ISP connector, the shield is not working on a Mega.  
The same way, if the SPI_CS is not on AVR SPI SS/CS, SPI just doesn't work.  
You have to set the custom CS pin as well as the standard AVR CS/SS pin to enable the SPI master to work.

### spi-speed
This program was written to verify with a logic analyzer that the SPI speed setting using the timer library was in the right range for an SD card.  
During the initialization sequence the SPI speed must be between 25 and 400 KHz. Once initialized it can be set to higher speed.  
For the slow speed I use 100/125 KHz and for the fast speed 1 MHz.

### dskbrowser
This application, written initially for a 6502 VHDL machine, opens an SD card in the SD card shield, lists all the files with extension `.INA` or `.DSK`.  
Then it offers to display the FLEX directory of these disk images, then offers to browse the blocks on one of the images.  
Note that this is a demonstration program and the block mapping is not reflecting FLEX block numbering.  
It could be improved to use FLEX T/S instead of block number.  
This program is the direct 6502 code with just an extra file `uart_console.c` added to be able to print/read from the serial port, and a call to `console_init()` added as well as a few `printf()` to print the AVR MCU used and the speed.
