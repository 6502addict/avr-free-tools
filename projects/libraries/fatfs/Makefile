# FatFs Library Makefile for AVR

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
CFLAGS += -Wno-unused-parameter -Wno-sign-compare

# Directories
BUILD_DIR = build
INSTALL_PREFIX = $(HOME)/avr/template
INCLUDE_DIR = $(INSTALL_PREFIX)/include
LIB_DIR = $(INSTALL_PREFIX)/lib

# Source files
SRC = ff.c diskio.c
OBJ = $(BUILD_DIR)/ff_$(MCU).o $(BUILD_DIR)/diskio_$(MCU).o
LIB = $(BUILD_DIR)/libfatfs_$(MCU).a

# Headers to install
HEADERS = ff.h ffconf.h diskio.h

# Default target
all: $(BUILD_DIR) $(LIB)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build object files
$(BUILD_DIR)/ff_$(MCU).o: ff.c ff.h ffconf.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I. -c ff.c -o $@

$(BUILD_DIR)/diskio_$(MCU).o: diskio.c diskio.h ff.h ffconf.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I. -c diskio.c -o $@

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
	@echo "Building for ATmega1284P..."
	@$(MAKE) MCU=atmega1284p F_CPU=16000000UL
	@echo ""
	@echo "Building for ATmega2560..."
	@$(MAKE) MCU=atmega2560 F_CPU=16000000UL

# Install library and headers
install: $(LIB) $(HEADERS)
	install -d $(INCLUDE_DIR)
	install -d $(LIB_DIR)
	install -m 644 $(HEADERS) $(INCLUDE_DIR)/
	install -m 644 $(LIB) $(LIB_DIR)/
	@echo "Installed $(LIB) to $(LIB_DIR)"
	@echo "Installed headers to $(INCLUDE_DIR)"

# Install all MCU libraries
install-all: all-mcus $(HEADERS)
	install -d $(INCLUDE_DIR)
	install -d $(LIB_DIR)
	install -m 644 $(HEADERS) $(INCLUDE_DIR)/
	install -m 644 $(BUILD_DIR)/libfatfs_atmega328p.a $(LIB_DIR)/
	install -m 644 $(BUILD_DIR)/libfatfs_atmega1284p.a $(LIB_DIR)/
	install -m 644 $(BUILD_DIR)/libfatfs_atmega2560.a $(LIB_DIR)/
	@echo "Installed all libraries to $(LIB_DIR)"
	@echo "Installed headers to $(INCLUDE_DIR)"

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Help
help:
	@echo "FatFs Library Makefile for AVR"
	@echo ""
	@echo "Usage:"
	@echo "  make                    - Build for default MCU ($(MCU))"
	@echo "  make MCU=atmega328p     - Build for ATmega328P"
	@echo "  make MCU=atmega1284p    - Build for ATmega1284P"
	@echo "  make MCU=atmega2560     - Build for ATmega2560"
	@echo "  make all-mcus           - Build for all supported MCUs"
	@echo "  make install            - Install library for current MCU"
	@echo "  make install-all        - Install libraries for all MCUs"
	@echo "  make clean              - Remove build files"
	@echo ""
	@echo "Current settings:"
	@echo "  MCU = $(MCU)"
	@echo "  F_CPU = $(F_CPU)"
	@echo ""
	@echo "Note: This library requires libspi_$(MCU).a and libtimer_$(MCU).a"
	@echo "      Make sure they are installed before linking applications"

.PHONY: all all-mcus clean install install-all help

