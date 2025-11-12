# Makefile for DS1302 RTC Test

# Project name
TARGET = ds1302-test

# Source files
SRC = ds1302-test.c uart_console.c

# MCU settings - default to Arduino Uno
MCU ?= atmega328p
F_CPU ?= 16000000UL
UART_BAUD ?= 9600

# Compiler and tools
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size

# Directories
LIB_DIR = $(HOME)/avr/template/lib
INCLUDE_DIR = $(HOME)/avr/template/include

# Compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DUART_BAUD=$(UART_BAUD) -Os -Wall -Wextra
CFLAGS += -std=gnu99 -I$(INCLUDE_DIR) -I.

# Linker flags
LDFLAGS = -mmcu=$(MCU)

# Libraries
LIBS = -L$(LIB_DIR) -lds1302_$(MCU)

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET).hex size

# Compile
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link
$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) $(LIBS) -o $@

# Create hex file
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Show size
size: $(TARGET).elf
	$(SIZE) --format=avr --mcu=$(MCU) $<

clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).o


# Build for all supported MCUs
all-mcus: clean
	@echo "Building for ATmega328P..."
	@$(MAKE) MCU=atmega328p F_CPU=16000000UL
	@mv $(TARGET).hex $(TARGET)_atmega328p.hex
	@mv $(TARGET).elf $(TARGET)_atmega328p.elf
	@$(MAKE) clean-obj
	@echo ""
	@echo "Building for ATmega1284P..."
	@$(MAKE) MCU=atmega1284p F_CPU=16000000UL
	@mv $(TARGET).hex $(TARGET)_atmega1284p.hex
	@mv $(TARGET).elf $(TARGET)_atmega1284p.elf
	@$(MAKE) clean-obj
	@echo ""
	@echo "Building for ATmega2560..."
	@$(MAKE) MCU=atmega2560 F_CPU=16000000UL
	@mv $(TARGET).hex $(TARGET)_atmega2560.hex
	@mv $(TARGET).elf $(TARGET)_atmega2560.elf
	@$(MAKE) clean-obj
	@echo "Building for attiny2313..."
	@$(MAKE) MCU=attiny2313 F_CPU=16000000UL
	@mv $(TARGET).hex $(TARGET)_atmega2313.hex
	@mv $(TARGET).elf $(TARGET)_atmega2313.elf
	@$(MAKE) clean-obj
	@echo

