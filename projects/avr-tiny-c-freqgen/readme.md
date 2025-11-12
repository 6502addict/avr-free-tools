this program use an avr tiny to generate a frequency a squarre signal

connect the serial port RX to PB4 and TX on PB3
then use a terminal 9600-8N1

you get a menu with 3 commands

frequency    xxxx        emit   a signal at xxxx frequency on PB0
stop                     stops the generator
status                   return the current frequency and the state of the generator stopped or runing

I used it to generate frequency   between 200hz and 10000 hz

the program can be adjusted to change the range

not that the precision is limited by the avr tiny clock speed
9.6Mhz on tiny13 8Mhz on tiny 25/45/85

