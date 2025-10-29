# FLEX Disk Image Explorer for AVR
# Makefile for ATmega328P/1284P/2560

# MCU settings
MCU ?= atmega2560
F_CPU ?= 16000000UL

# Serial port settings for console
BAUD ?= 38400

# Compiler and tools
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
AVRDUDE = avrdude

# Compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DBAUD=$(BAUD)
CFLAGS += -Os -Wall -Wextra
CFLAGS += -std=gnu99 -ffunction-sections -fdata-sections
CFLAGS += -Wno-unused-parameter
CFLAGS += -flto  # Link-time optimization for smaller code

# Linker flags
LDFLAGS = -mmcu=$(MCU)
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref
LDFLAGS += -flto  # Link-time optimization

# Directories
BUILD_DIR = build
LIB_DIR = $(HOME)/avr/template/lib
INCLUDE_DIR = $(HOME)/avr/template/include

# Include paths
INCLUDES = -I$(INCLUDE_DIR)

# Libraries (order matters! timer must be last)
LIBS = -L$(LIB_DIR)
LIBS += -lfatfs_$(MCU)
LIBS += -lsdcard_$(MCU)
LIBS += -lspi_$(MCU)
LIBS += -ltimer_$(MCU)

# Source files
SRC = dskbrowser.c uart_console.c
HEADERS = uart_console.h

# Object files
OBJ = $(BUILD_DIR)/dskbrowser.o $(BUILD_DIR)/uart_console.o

# Target files
TARGET = dskbrowser
ELF = $(BUILD_DIR)/$(TARGET).elf
HEX = $(BUILD_DIR)/$(TARGET).hex
BIN = $(BUILD_DIR)/$(TARGET).bin

# Programmer settings (adjust for your programmer)
PROGRAMMER ?= arduino
PORT ?= /dev/ttyUSB0

# Default target
all: $(BUILD_DIR) $(HEX) size

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile dskbrowser.c
$(BUILD_DIR)/dskbrowser.o: dskbrowser.c uart_console.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile uart_console.c
$(BUILD_DIR)/uart_console.o: uart_console.c uart_console.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link
$(ELF): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) $(LIBS) -o $@

# Create hex file
$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	@echo ""
	@echo "Hex file created: $@"

# Create bin file
$(BIN): $(ELF)
	$(OBJCOPY) -O binary -R .eeprom $< $@

# Show size
size: $(ELF)
	@echo ""
	@echo "Size information:"
	$(SIZE) --format=avr --mcu=$(MCU) $<
	@echo ""

# Upload to MCU
upload: $(HEX)
	$(AVRDUDE) -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -U flash:w:$<:i

# Build for all supported MCUs
all-mcus: clean
	@echo "Building for ATmega328P..."
	@$(MAKE) MCU=atmega328p
	@echo ""
	@echo "Building for ATmega1284P..."
	@$(MAKE) MCU=atmega1284p
	@echo ""
	@echo "Building for ATmega2560..."
	@$(MAKE) MCU=atmega2560

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Help
help:
	@echo "FLEX Disk Image Explorer for AVR"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build for current MCU ($(MCU))"
	@echo "  upload       - Upload to MCU"
	@echo "  all-mcus     - Build for all supported MCUs"
	@echo "  clean        - Remove build files"
	@echo "  size         - Show size information"
	@echo ""
	@echo "Configuration:"
	@echo "  MCU=$(MCU)"
	@echo "  F_CPU=$(F_CPU)"
	@echo "  BAUD=$(BAUD)"
	@echo "  PROGRAMMER=$(PROGRAMMER)"
	@echo "  PORT=$(PORT)"
	@echo ""
	@echo "Examples:"
	@echo "  make MCU=atmega328p"
	@echo "  make MCU=atmega1284p BAUD=115200"
	@echo "  make upload PORT=/dev/ttyACM0"
	@echo ""
	@echo "Note: ATmega328P has limited RAM (2KB)"
	@echo "      Use ATmega1284P (16KB RAM) for better performance"

.PHONY: all all-mcus clean upload size help
