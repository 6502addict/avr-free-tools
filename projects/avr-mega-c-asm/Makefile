# Makefile for Mixed C/Assembly Hex Display
# Supports: ATmega328P, ATmega2560, ATmega1284P

# Target chip selection
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
    F_CPU = 8000000UL
    BAUD = 38400
endif

# Project name
TARGET = serial_echo_hex

# Source files
C_SRC = $(TARGET).c
ASM_SRC = bin_to_hex.S

# Compiler and tools
CC = avr-gcc
AS = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
AVRDUDE = avrdude

# Compiler flags for C files
CFLAGS = -mmcu=$(MCU)
CFLAGS += -DF_CPU=$(F_CPU)
CFLAGS += -DBAUD=$(BAUD)
CFLAGS += -Os
CFLAGS += -Wall -Wextra
CFLAGS += -std=c99
CFLAGS += -funsigned-char -funsigned-bitfields
CFLAGS += -fpack-struct -fshort-enums
CFLAGS += -ffunction-sections -fdata-sections

# Assembler flags
ASFLAGS = -mmcu=$(MCU)
ASFLAGS += -x assembler-with-cpp
ASFLAGS += -Wa,-gstabs

# Linker flags
LDFLAGS = -mmcu=$(MCU)
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--relax
LDFLAGS += -Wl,-Map=$(TARGET).map

# Avrdude flags
AVRDUDE_FLAGS = -p $(MCU) -c $(PROGRAMMER)
AVRDUDE_FLAGS += -P /dev/ttyUSB0

# Object files
C_OBJ = $(C_SRC:.c=.o)
ASM_OBJ = $(ASM_SRC:.S=.o)
OBJ = $(C_OBJ) $(ASM_OBJ)

# Default target
all: $(TARGET).hex size

# Compile C files
%.o: %.c
	@echo "Compiling C: $<"
	$(CC) $(CFLAGS) -c -o $@ $<

# Assemble ASM files
%.o: %.S
	@echo "Assembling: $<"
	$(AS) $(ASFLAGS) -c -o $@ $<

# Link all object files
$(TARGET).elf: $(OBJ)
	@echo "Linking: $@"
	$(CC) $(LDFLAGS) -o $@ $^
	@echo ""

# Create hex file
$(TARGET).hex: $(TARGET).elf
	@echo "Creating hex file: $@"
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	@echo ""

# Create EEPROM file (if needed)
$(TARGET).eep: $(TARGET).elf
	@echo "Creating EEPROM file: $@"
	$(OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 $< $@

# Display size information
size: $(TARGET).elf
	@echo "Size information:"
	$(SIZE) --format=avr --mcu=$(MCU) $<
	@echo ""

# Upload to chip
upload: $(TARGET).hex
	@echo "Uploading to $(MCU)..."
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(TARGET).hex:i

# Upload with verbose output
upload-verbose: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -v -U flash:w:$(TARGET).hex:i

# Read fuses
read-fuses:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h

# Disassemble
disasm: $(TARGET).elf
	avr-objdump -d -S $< > $(TARGET).lss

# Clean
clean:
	@echo "Cleaning..."
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).eep $(TARGET).map $(TARGET).lss
	rm -f $(OBJ)
	rm -f *.o

# Terminal for testing (requires screen)
terminal:
	@echo "Opening terminal (115200 8N1)..."
	@echo "Exit with: Ctrl-A then K"
	screen $(PORT) $(BAUD)

# Generate assembly listing from C
%.s: %.c
	$(CC) -S $(CFLAGS) $< -o $@

# Build for all chips
all-mcus:
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
	@echo "Mixed C/Assembly Hex Display Build System"
	@echo "========================================="
	@echo ""
	@echo "This program receives bytes via UART and displays them as hexadecimal"
	@echo "Format: XX XX XX XX XX XX XX XX<CR><LF>"
	@echo ""
	@echo "Targets:"
	@echo "  make              - Build the project"
	@echo "  make upload       - Upload to MCU"
	@echo "  make terminal     - Open serial terminal"
	@echo "  make size         - Show memory usage"
	@echo "  make disasm       - Create disassembly listing"
	@echo "  make clean        - Remove build files"
	@echo "  make read-fuses   - Read MCU fuse settings"
	@echo ""
	@echo "Options:"
	@echo "  MCU=atmega328p    - Target MCU (default)"
	@echo "  MCU=atmega2560    - ATmega2560"
	@echo "  MCU=atmega1284p   - ATmega1284P"
	@echo ""
	@echo "Example:"
	@echo "  make MCU=atmega2560 upload terminal"
	@echo ""
	@echo "Files:"
	@echo "  serial_hex.c      - Main C code (UART handling)"
	@echo "  bin_to_hex.S      - Assembly code (hex conversion)"

.PHONY: all clean upload upload-verbose size terminal help disasm read-fuses

