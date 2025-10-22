# Makefile
# Multi-AVR Serial Echo
# Supports: ATmega328P, ATmega2560, ATmega1284P

# Target chip selection (change this to build for different chips)
# Options: attiny85, atmega328p, atmega2560, atmega1284p
MCU ?= atmega328p

PROGRAMMER = avrispmkii

# Chip-specific settings
ifeq ($(MCU),atmega328p)
    F_CPU = 16000000UL
    BAUD = 38400
endif

ifeq ($(MCU),atmega2560)
    F_CPU = 16000000UL
    BAUD = 38400
endif

ifeq ($(MCU),atmega1284p)
    F_CPU = 16000000UL
    BAUD = 38400
endif

# Project name
TARGET = serial_echo

# Source files
SRC = $(TARGET).c

# Compiler and tools
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
AVRDUDE = avrdude

# Compiler flags
CFLAGS = -mmcu=$(MCU)
CFLAGS += -DF_CPU=$(F_CPU)
CFLAGS += -DBAUD=$(BAUD)
CFLAGS += -Os
CFLAGS += -Wall -Wextra
CFLAGS += -std=c99
CFLAGS += -funsigned-char -funsigned-bitfields
CFLAGS += -fpack-struct -fshort-enums
CFLAGS += -ffunction-sections -fdata-sections

# Linker flags
LDFLAGS = -mmcu=$(MCU)
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--relax

# Avrdude flags
AVRDUDE_FLAGS = -p $(MCU) -c $(PROGRAMMER)
AVRDUDE_FLAGS += -P /dev/ttyUSB0

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET).hex size

# Compile
%.o: %.c
	@echo "Compiling for $(MCU)..."
	$(CC) $(CFLAGS) -c -o $@ $<

# Link
$(TARGET).elf: $(OBJ)
	@echo "Linking..."
	$(CC) $(LDFLAGS) -o $@ $^

# Create hex file
$(TARGET).hex: $(TARGET).elf
	@echo "Creating hex file..."
	$(OBJCOPY) -O ihex $< $@

# Show size
size: $(TARGET).elf
	@echo "Size information:"
	$(SIZE) --format=avr --mcu=$(MCU) $<

# Upload to chip
upload: $(TARGET).hex
	@echo "Uploading to $(MCU)..."
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(TARGET).hex:i

# Clean
clean:
	@echo "Cleaning..."
	rm -f $(TARGET).elf $(TARGET).hex $(OBJ)

# Build for all chips
all-chips:
	@echo "Building for all chips..."
	@make clean
	@make MCU=atmega328p
	@mv $(TARGET).hex $(TARGET)_328p.hex
	@make clean
	@make MCU=atmega2560
	@mv $(TARGET).hex $(TARGET)_2560.hex
	@make clean
	@make MCU=atmega1284p
	@mv $(TARGET).hex $(TARGET)_1284p.hex
	@echo "Done! Created: $(TARGET)_328p.hex, $(TARGET)_2560.hex, $(TARGET)_1284p.hex"

# Help
help:
	@echo "Multi-AVR Serial Echo Build System"
	@echo ""
	@echo "Usage:"
	@echo "  make MCU=atmega328p      - Build for ATmega328P (default)"
	@echo "  make MCU=atmega2560      - Build for ATmega2560"
	@echo "  make MCU=atmega1284p     - Build for ATmega1284P"
	@echo "  make upload              - Upload to chip"
	@echo "  make all-chips           - Build for all chips"
	@echo "  make clean               - Remove build files"
	@echo ""
	@echo "Example:"
	@echo "  make MCU=atmega2560 upload"

.PHONY: all clean upload size help all-chips
