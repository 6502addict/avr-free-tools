MCU = attiny13
F_CPU = 9600000UL
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size

CFLAGS = -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU)
CFLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections

TARGET = freq_gen
SOURCE = freq_gen.c

all: $(TARGET).hex size

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

$(TARGET).elf: $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $<

size: $(TARGET).elf
	$(SIZE) --mcu=$(MCU) --format=avr $<

flash: $(TARGET).hex
	avrdude -c usbasp -p t13 -U flash:w:$<

# Set fuses for 9.6MHz internal oscillator
fuse:
	avrdude -c usbasp -p t13 -U lfuse:w:0x7a:m -U hfuse:w:0xff:m

clean:
	rm -f *.hex *.elf *.o

.PHONY: all size flash fuse clean

