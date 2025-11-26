# Project name
TARGET = echo

# Source files
SRC = echo.c

# MCU settings - default to Arduino Uno
MCU ?= attiny13
F_CPU ?= 9600000UL
UART_BAUD ?= 9600

# Compiler and tools
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size

# Directories
LIB_DIR = $(HOME)/avr/template/lib
INCLUDE_DIR = $(HOME)/avr/template/include

# Compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DBAUD_RATE=$(UART_BAUD) -Os -Wall -Wextra
CFLAGS += -std=gnu99 -I$(INCLUDE_DIR) -I.

# Linker flags
LDFLAGS = -mmcu=$(MCU)

# Libraries
LIBS = -L$(LIB_DIR) -lds1302_$(MCU) -luart-tiny_$(MCU)

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
# Build for all supported MCUs
all-mcus: clean
	@echo "Building for ATtiny13..."
	@$(MAKE) MCU=attiny13 F_CPU=9600000UL UART_BAUD=1200
	@mv $(TARGET).hex $(TARGET)_attiny13.hex
	@mv $(TARGET).elf $(TARGET)_attiny13.elf
	@rm -f $(OBJ)
	@echo ""
	@echo "Building for ATtiny25..."
	@$(MAKE) MCU=attiny25 F_CPU=8000000UL UART_BAUD=1200
	@mv $(TARGET).hex $(TARGET)_attiny25.hex
	@mv $(TARGET).elf $(TARGET)_attiny25.elf
	@rm -f $(OBJ)
	@echo ""
	@echo "Building for ATtiny45..."
	@$(MAKE) MCU=attiny45 F_CPU=8000000UL UART_BAUD=1200
	@mv $(TARGET).hex $(TARGET)_attiny45.hex
	@mv $(TARGET).elf $(TARGET)_attiny45.elf
	@rm -f $(OBJ)
	@echo ""
	@echo "Building for ATtiny85..."
	@$(MAKE) MCU=attiny85 F_CPU=8000000UL UART_BAUD=1200
	@mv $(TARGET).hex $(TARGET)_attiny85.hex
	@mv $(TARGET).elf $(TARGET)_attiny85.elf
	@rm -f $(OBJ)
	@echo ""
	@echo "Building for ATtiny2313..."
	@$(MAKE) MCU=attiny2313 F_CPU=8000000UL UART_BAUD=1200
	@mv $(TARGET).hex $(TARGET)_attiny2313.hex
	@mv $(TARGET).elf $(TARGET)_attiny2313.elf
	@rm -f $(OBJ)
	@echo "Done!"

