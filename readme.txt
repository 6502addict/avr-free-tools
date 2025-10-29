these projects are essemtialy a way to have something to start from for another project
as the serial port is often used in many project
they are implementing an echo on TX of what is received on RX from the serial port
on avr atmega the serial port is a real serial port on the attiny it is emulated so the speed can't be pushed really far
the avr-mega-c-asm is a variation on other project it display what is received on RX but in hexadecimal 2 bytes
followed by a blank and a cr/lf after the 8th byte... the purpose of this project is to show the gcc/avra integration
the c handling the serial port and the assembler handling the conversion from binary to hexadecimal

most of the documentations were written by IA so if you find errors report the to didier@aida.org I'll fix them

most of the packages can be installed directly from the linux package
but for avra some include files are missing  so I remove the avra package and built avra from github:  https://github.com/Ro5bert/avra
