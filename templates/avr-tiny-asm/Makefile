# ATtiny Software Serial Echo Makefile (AVRA Assembler)
# Supports: ATtiny13, ATtiny25, ATtiny45, ATtiny85

# Target chip selection
MCU ?= ATtiny13

# Programmer types:
# USBtinyISP     usbtiny
# AVR ISP MKII   avrispmkii
# Atmel ICE      atmelice_isp
PROGRAMMER = avrispmkii

# Chip-specific settings
ifeq ($(MCU),attiny13)
    ASMFLAGS = -D ATtiny13
    AVRDUDE_MCU = t13
    # ATtiny13 has only 1KB flash, 64B RAM
endif

ifeq ($(MCU),attiny25)
    ASMFLAGS = -D ATtiny25
    AVRDUDE_MCU = t25
    # ATtiny25 has 2KB flash, 128B RAM
endif

ifeq ($(MCU),attiny45)
    ASMFLAGS = -D ATtiny45
    AVRDUDE_MCU = t45
    # ATtiny45 has 4KB flash, 256B RAM
endif

ifeq ($(MCU),attiny85)
    ASMFLAGS = -D ATtiny85
    AVRDUDE_MCU = t85
    # ATtiny85 has 8KB flash, 512B RAM
endif

# Project name
TARGET = uart_echo_tiny
SRC = $(TARGET).asm

# Tools
ASM = avra
AVRDUDE = avrdude

# Assembler flags
ASMFLAGS += -fI
ASMFLAGS += -I /usr/local/include/avr
ASMFLAGS += -D F_CPU=$(F_CPU)
ASMFLAGS += -D BAUD=$(BAUD)

# Avrdude flags
AVRDUDE_FLAGS = -p $(AVRDUDE_MCU) -c $(PROGRAMMER)

# Default target
all: $(TARGET).hex

# Assemble
$(TARGET).hex: $(SRC)
	@echo "======================================="
	@echo "Assembling for $(MCU)"
	@echo "F_CPU: $(F_CPU) Hz"
	@echo "BAUD: $(BAUD)"
	@echo "======================================="
	$(ASM) $(ASMFLAGS) $(SRC) -o $(TARGET).hex
	@if [ -f $(TARGET).eep.hex ]; then \
		echo "EEPROM file created: $(TARGET).eep.hex"; \
	fi
	@echo "Output: $(TARGET).hex"

# Upload to chip
upload: $(TARGET).hex
	@echo "Uploading to $(MCU)..."
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(TARGET).hex:i

# Fuse settings for 8MHz external crystal
fuses-8mhz-ext:
	@echo "Setting fuses for $(MCU) 8MHz external crystal..."
ifeq ($(MCU),ATtiny13)
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U lfuse:w:0x7F:m \
		-U hfuse:w:0xFF:m
endif
ifeq ($(MCU),ATtiny25)
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U lfuse:w:0xFF:m \
		-U hfuse:w:0xDF:m \
		-U efuse:w:0xFF:m
endif
ifeq ($(MCU),ATtiny45)
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U lfuse:w:0xFF:m \
		-U hfuse:w:0xDF:m \
		-U efuse:w:0xFF:m
endif
ifeq ($(MCU),ATtiny85)
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U lfuse:w:0xFF:m \
		-U hfuse:w:0xDF:m \
		-U efuse:w:0xFF:m
endif

# Fuse settings for 8MHz internal oscillator
fuses-8mhz-int:
	@echo "Setting fuses for $(MCU) 8MHz internal oscillator..."
ifeq ($(MCU),ATtiny13)
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U lfuse:w:0x7A:m \
		-U hfuse:w:0xFF:m
endif
ifeq ($(MCU),ATtiny25)
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U lfuse:w:0xE2:m \
		-U hfuse:w:0xDF:m \
		-U efuse:w:0xFF:m
endif
ifeq ($(MCU),ATtiny45)
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U lfuse:w:0xE2:m \
		-U hfuse:w:0xDF:m \
		-U efuse:w:0xFF:m
endif
ifeq ($(MCU),ATtiny85)
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U lfuse:w:0xE2:m \
		-U hfuse:w:0xDF:m \
		-U efuse:w:0xFF:m
endif

# Clean
clean:
	@echo "Cleaning..."
	rm -f $(TARGET).hex $(TARGET).eep.hex $(TARGET).obj $(TARGET).cof

# Build for all ATtiny chips
all-chips:
	@echo "Building for all ATtiny chips..."
	@make clean
	@make MCU=attiny13
	@mv $(TARGET).hex $(TARGET)_t13.hex
	@make clean
	@make MCU=attiny25
	@mv $(TARGET).hex $(TARGET)_t25.hex
	@make clean
	@make MCU=attiny45
	@mv $(TARGET).hex $(TARGET)_t45.hex
	@make clean
	@make MCU=attiny85
	@mv $(TARGET).hex $(TARGET)_t85.hex
	@echo "======================================="
	@echo "Done! Created:"
	@echo "  $(TARGET)_t13.hex (1KB flash)"
	@echo "  $(TARGET)_t25.hex (2KB flash)"
	@echo "  $(TARGET)_t45.hex (4KB flash)"
	@echo "  $(TARGET)_t85.hex (8KB flash)"
	@echo "======================================="

# Test serial connection
test:
	@echo "Testing serial connection..."
	@echo "TX: PB1 (pin 6), RX: PB0 (pin 5)"
	@echo "Baud: $(BAUD), 8N1"
	@echo "Connect with: screen /dev/ttyUSB0 $(BAUD)"

# Help
help:
	@echo "ATtiny Software Serial Build System"
	@echo "===================================="
	@echo ""
	@echo "Supported chips:"
	@echo "  ATtiny13 - 1KB flash, 64B RAM"
	@echo "  ATtiny25 - 2KB flash, 128B RAM"
	@echo "  ATtiny45 - 4KB flash, 256B RAM"
	@echo "  ATtiny85 - 8KB flash, 512B RAM"
	@echo ""
	@echo "Usage:"
	@echo "  make MCU=ATtiny85        - Build for ATtiny85 (default)"
	@echo "  make MCU=ATtiny45        - Build for ATtiny45"
	@echo "  make MCU=ATtiny25        - Build for ATtiny25"
	@echo "  make MCU=ATtiny13        - Build for ATtiny13"
	@echo "  make upload              - Upload to chip"
	@echo "  make fuses-8mhz-ext      - Set fuses for 8MHz external crystal"
	@echo "  make fuses-8mhz-int      - Set fuses for 8MHz internal oscillator"
	@echo "  make all-tiny            - Build for all ATtiny chips"
	@echo "  make test                - Show test connection info"
	@echo "  make clean               - Remove build files"
	@echo ""
	@echo "Pin connections:"
	@echo "  TX: PB1 (physical pin 6)"
	@echo "  RX: PB0 (physical pin 5)"
	@echo ""
	@echo "Example:"
	@echo "  make MCU=ATtiny45 fuses-8mhz-ext upload"

.PHONY: all clean upload help all-tiny test fuses-8mhz-ext fuses-8mhz-int
