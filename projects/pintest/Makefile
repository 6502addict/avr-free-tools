# Makefile for Arduino Mega 2560 (ATmega2560)

# Project name
TARGET = pintest

# Source files
SRC = pintest.c

# MCU settings
MCU = atmega2560
F_CPU = 16000000UL

# Programmer settings (adjust COM port for your system)
PROGRAMMER = wiring
PORT = /dev/ttyACM0
BAUD = 115200

# Compiler and tools
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
AVRDUDE = avrdude

# Compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra
CFLAGS += -std=gnu99

# Linker flags
LDFLAGS = -mmcu=$(MCU)

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET).hex size

# Compile
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link
$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

# Create hex file
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Show size
size: $(TARGET).elf
	$(SIZE) --format=avr --mcu=$(MCU) $<

# Upload to Arduino Mega
upload: $(TARGET).hex
	$(AVRDUDE) -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -U flash:w:$<:i

# Clean
clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).hex

.PHONY: all size upload clean

