# Project name
TARGET = serial_echo

# Source files
SRC = serial_echo.c

# MCU settings - default to Arduino Uno
MCU ?= atmega328p
F_CPU ?= 9600000UL
BAUD ?= 9600

# Compiler and tools
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size

# Directories
LIB_DIR = $(HOME)/avr/template/lib
INCLUDE_DIR = $(HOME)/avr/template/include

# Compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -Os -Wall -Wextra
CFLAGS += -std=gnu99 -I$(INCLUDE_DIR) -I.

# Linker flags
LDFLAGS = -mmcu=$(MCU)

# Libraries
LIBS = -L$(LIB_DIR) -luart-mega_$(MCU)

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
	@echo "Building for ATmega328p..."
	@$(MAKE) MCU=atmega328p F_CPU=1600000UL BAUD=9600
	@mv $(TARGET).hex $(TARGET)_atmega328p.hex
	@mv $(TARGET).elf $(TARGET)_atmega328p.elf
	@rm -f $(OBJ)
	@echo ""
	@echo "Building for ATmega1284p..."
	@$(MAKE) MCU=atmega1284p F_CPU=16000000UL BAUD=9600
	@mv $(TARGET).hex $(TARGET)_atmega1284.hex
	@mv $(TARGET).elf $(TARGET)_atmega1284.elf
	@rm -f $(OBJ)
	@echo ""
	@echo "Building for ATmega2560..."
	@$(MAKE) MCU=atmega2560 F_CPU=8000000UL BAUD=9600
	@mv $(TARGET).hex $(TARGET)_atmega2560.hex
	@mv $(TARGET).elf $(TARGET)_atmega2560.elf
	@rm -f $(OBJ)
	@echo "Done!"

