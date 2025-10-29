# Libraries 

contains my 6502 libraries ported to avr

##*timer*   implement my timer library on 6502 for an avr by using an avr timer to implement the functions
          but with exactly the same interface...

##*spi*               implement my spi library for 6502 for an avr by using the avr master spi to implement the functions
                    but with exactly the same interface...

##*sdcard*            is the exact code used on the 6502 it depends only on the spi and timer libraries

##*fatfs*             is the standard fatfs library without any modification
                    diskio.c is adapted to use my sdcard library

# Template

the template are 4 projects to help build a custom project

as many people need to use a serial port in avr project and as I want to keep this projects simple
all these project implement an echo on TX on what is received on RX of the serial port 0 (if several)


*avr-tiny-asm*       as flash / ram of the avr tiny are very limited on avr tiny... so I've chosen to create a project using avra
                     avra is a free avr assembler very close to orginal avrasm from atmel
                     so old code should be easy to port with this template
                     on this project the serial port is emulated and the speed limited
                     note that by like of hardware this project is not fully tested (it will be soon)

*avr-mega-asm*       this is roughly the same idea as for the avr tiny but this time uses the real serial port 0
                     it was fully tested on arduino uno,  arduino mega 2560, and on a proto board with an atmega1284                
                     (the atmega1284 is a nice chip with 128k flash 16k ram, and support debugging via jtag)
                     choose this template if you want to recover old ave code for avr atmega

*avr-mega-c*         same thing that avr-mega-asm execept that this time the code is in pure C (gcc)  not the awful arduino code
                     it was fully tested on arduino uno,  arduino mega 2560, and on a proto board with an atmega1284                
                     choose this template if you want to build a pure C project
                      
*avr-mega-c-asm*     same as a above but the goal of this template is to show how to mix C (gcc c) and assembler
                     note that avra cannot be used, this time the assembler program linked to the c must by assemnled by the gcc assembler
                     the syntax is different fromt the standard avr assembler... check carefully the differences to avoid problems
                     there is a small difference in this project it still echo the data received by this time in hexadecimal
                     the conversion is done by the assembly program...
                     it was fully tested on arduino uno,  arduino mega 2560, and on a proto board with an atmega1284                
                     choose this template if you want to build a mixed C/Assembly project

# Tests

theses tests where written to test some functions on the avr / spi / timer

*blink*              just blink the led of an arduino / proto board
                     this program was also written to verify that time timer library was also working
                     the timer library being used for the delay between 2 led states change

*spi-test*           small test to verify that spi worked on the arduino mega 
                     if you read the arduino documentation you have the feeling that the arduino shields are compatible between the uno and mega
                     and fact it is not the case apparently pin N arrives on pin N  but they are not mapped the same way
                     on D10/11/12/13 you have spi on the uno but it's not the case on the mega so if your sdcard shield is not taking spi on the isp connector
                     the shield is not working on a mega.... the same way if the SPI_CS is not on AVR SPI SS/CS spi just don't works
                     you have to set the custom cs pin as well as the standard avr cs/ss pin to enable the spi master to work

*spi-speed*          this program was written to verify with a logic analyzer that the spi speed setting using the timer library was in the right range for a sdcard
                     during the initialisation sequence the spi speed must be between 25 and 400Khz then once initialized it can be set to higher speed
                     for the slow speed I use 100/125Khz and for the fast speed 1Mhz

*dskbrowser*         this application written initially for a 6502 vhdl machine open a sdcard in the sdcard shield, list all the files with extension .INA or .DSK
                     the it offer to display the flex directory of these disk images... then offer to browse the blocks on one of the images
                     note that this is a demonstration program  and the block mapping is not reflecting flex block numbering
                     it could be improved to use flex T/S instead of block number
                     this program is the direct 6502 with just an extra file uart_console.c added to be able to print / read from the serial port..
                     and a call to console_init() added as well as a flew printf to print the avr mcu used and the speed.


                     
                     
                     
                     

          
      
