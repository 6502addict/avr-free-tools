# Libraries 

contains my 6502 libraries ported to avr

*timer*   implement my timer library on 6502 for an avr by using an avr timer to implement the functions
          but with exactly the same interface...

*spi*               implement my spi library for 6502 for an avr by using the avr master spi to implement the functions
                    but with exactly the same interface...

*sdcard*            is the exact code used on the 6502 it depends only on the spi and timer libraries

*fatfs*             is the standard fatfs library without any modification
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


          
      
