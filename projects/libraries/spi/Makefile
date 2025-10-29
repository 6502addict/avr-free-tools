# AVR SPI Library Makefile

# MCU settings - change this to build for different targets
MCU ?= atmega328p
F_CPU ?= 16000000UL

# Compiler and tools
CC = avr-gcc
AR = avr-ar
SIZE = avr-size

# Compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra
CFLAGS += -std=gnu99 -ffunction-sections -fdata-sections

# Directories
BUILD_DIR = build
INSTALL_PREFIX = $(HOME)/avr/template
INCLUDE_DIR = $(INSTALL_PREFIX)/include
LIB_DIR = $(INSTALL_PREFIX)/lib

# Source files
SRC = spi.c
OBJ = $(BUILD_DIR)/spi_$(MCU).o
LIB = $(BUILD_DIR)/libspi_$(MCU).a

# Default target
all: $(BUILD_DIR) $(LIB)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build object file
$(BUILD_DIR)/spi_$(MCU).o: spi.c spi.h
	$(CC) $(CFLAGS) -c spi.c -o $@

# Build library
$(LIB): $(OBJ)
	$(AR) rcs $(LIB) $(OBJ)
	@echo "Library created: $(LIB)"
	$(SIZE) $(LIB)

# Build for all supported MCUs
all-mcus: clean
	@echo "Building for ATmega328P..."
	@$(MAKE) MCU=atmega328p F_CPU=16000000UL
	@echo ""
	@echo "Building for ATmega1284..."
	@$(MAKE) MCU=atmega1284 F_CPU=16000000UL
	@echo ""
	@echo "Building for ATmega1284P..."
	@$(MAKE) MCU=atmega1284p F_CPU=16000000UL
	@echo ""
	@echo "Building for ATmega2560..."
	@$(MAKE) MCU=atmega2560 F_CPU=16000000UL

# Install library and header
install: $(LIB) spi.h
	install -d $(INCLUDE_DIR)
	install -d $(LIB_DIR)
	install -m 644 spi.h $(INCLUDE_DIR)/
	install -m 644 $(LIB) $(LIB_DIR)/
	@echo "Installed $(LIB) to $(LIB_DIR)"
	@echo "Installed spi.h to $(INCLUDE_DIR)"

# Install all MCU libraries
install-all: all-mcus spi.h
	install -d $(INCLUDE_DIR)
	install -d $(LIB_DIR)
	install -m 644 spi.h $(INCLUDE_DIR)/
	install -m 644 $(BUILD_DIR)/libspi_atmega328p.a $(LIB_DIR)/
	install -m 644 $(BUILD_DIR)/libspi_atmega1284.a $(LIB_DIR)/
	install -m 644 $(BUILD_DIR)/libspi_atmega1284p.a $(LIB_DIR)/
	install -m 644 $(BUILD_DIR)/libspi_atmega2560.a $(LIB_DIR)/
	@echo "Installed all libraries to $(LIB_DIR)"
	@echo "Installed spi.h to $(INCLUDE_DIR)"

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Help
help:
	@echo "AVR SPI Library Makefile"
	@echo ""
	@echo "Usage:"
	@echo "  make                    - Build for default MCU ($(MCU))"
	@echo "  make MCU=atmega328p     - Build for ATmega328P"
	@echo "  make MCU=atmega1284     - Build for ATmega1284"
	@echo "  make MCU=atmega1284p    - Build for ATmega1284P"
	@echo "  make MCU=atmega2560     - Build for ATmega2560"
	@echo "  make MCU=attiny2313     - Build for ATtiny2313"
	@echo "  make all-mcus           - Build for all supported MCUs"
	@echo "  make install            - Install library"
	@echo "  make clean              - Remove build files"
	@echo ""
	@echo "Current settings:"
	@echo "  MCU = $(MCU)"
	@echo "  F_CPU = $(F_CPU)"

.PHONY: all all-mcus clean install help
