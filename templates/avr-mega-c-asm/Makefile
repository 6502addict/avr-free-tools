# Project settings
TARGET = serial_echo_hex
C_SRC = $(TARGET).c
ASM_SRC = bin_to_hex.S
SRC = $(C_SRC) $(ASM_SRC)

# MCU settings
MCU ?= atmega328p
F_CPU ?= 16000000UL
UART_BAUD ?= 9600

# Chip-specific settings (optional overrides)
ifeq ($(MCU),atmega328p)
    F_CPU = 16000000UL
endif

ifeq ($(MCU),atmega2560)
    F_CPU = 16000000UL
endif

ifeq ($(MCU),atmega1284p)
    F_CPU = 8000000UL
endif

# Standard paths
LIB_DIR = $(HOME)/avr/template/lib
INCLUDE_DIR = $(HOME)/avr/template/include

# Tools
CC = avr-gcc
AS = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size

# Compiler flags
CFLAGS = -mmcu=$(MCU) -D$(MCU) -DF_CPU=$(F_CPU) -DBAUD=$(BAUD)
CFLAGS += -Os -Wall -Wextra -std=gnu99 -I$(INCLUDE_DIR) -I.
CFLAGS += -ffunction-sections -fdata-sections

# Assembler flags
ASFLAGS = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs

# Linker flags
LDFLAGS = -mmcu=$(MCU) -Wl,--gc-sections -Wl,--relax

# Libraries
LIBS = -L$(LIB_DIR) -luart-mega_$(MCU)

# Object files
C_OBJ = $(C_SRC:.c=.o)
ASM_OBJ = $(ASM_SRC:.S=.o)
OBJ = $(C_OBJ) $(ASM_OBJ)

# Default target
all: $(TARGET).hex size

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM files
%.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

# Link
$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) $(LIBS) -o $@

# Create hex file
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Show size
size: $(TARGET).elf
	@echo "Flash and RAM usage:"
	$(SIZE) --format=avr --mcu=$(MCU) $<

# Disassemble
disasm: $(TARGET).elf
	avr-objdump -d -S $< > $(TARGET).lss

# Clean
clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).lss $(TARGET).map
	rm -f $(OBJ)

# Build for all supported MCUs
all-mcus: clean
	@echo "Building for ATmega328P..."
	@$(MAKE) MCU=atmega328p BAUD=9600
	@mv $(TARGET).hex $(TARGET)_328p.hex
	@rm -f $(OBJ)
	@echo ""
	@echo "Building for ATmega2560..."
	@$(MAKE) MCU=atmega2560 BAUD=9600
	@mv $(TARGET).hex $(TARGET)_2560.hex
	@rm -f $(OBJ)
	@echo ""
	@echo "Building for ATmega1284P..."
	@$(MAKE) MCU=atmega1284p BAUD=9600
	@mv $(TARGET).hex $(TARGET)_1284p.hex
	@rm -f $(OBJ)
	@echo "Done! Created: $(TARGET)_328p.hex, $(TARGET)_2560.hex, $(TARGET)_1284p.hex"

# Help
help:
	@echo "Hex Display Build System"
	@echo "========================"
	@echo ""
	@echo "Targets:"
	@echo "  make              - Build for default MCU"
	@echo "  make all-mcus     - Build for all MCUs"
	@echo "  make size         - Show memory usage"
	@echo "  make disasm       - Create disassembly"
	@echo "  make clean        - Remove build files"
	@echo ""
	@echo "Options:"
	@echo "  MCU=atmega328p    - ATmega328P (default)"
	@echo "  MCU=atmega2560    - ATmega2560"
	@echo "  MCU=atmega1284p   - ATmega1284P"
	@echo "  UART_BAUD=38400   - Baud rate (default)"

.PHONY: all size clean disasm all-mcus help
