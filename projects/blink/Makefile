# Arduino Blink Examples Makefile

# Tools
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
AVRDUDE = avrdude

# Common flags
CFLAGS = -Os -Wall -Wextra -std=gnu99 -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections

# Timer library path
TIMER_LIB_DIR = $(HOME)/avr/template/lib
TIMER_INCLUDE = $(HOME)/avr/template/include

# Arduino Uno settings
UNO_MCU = atmega328p
UNO_F_CPU = 16000000UL
UNO_BAUD = 115200

# ATmega1284P settings
M1284_MCU = atmega1284p
M1284_F_CPU = 16000000UL
M1284_BAUD = 115200

# Arduino Mega settings
MEGA_MCU = atmega2560
MEGA_F_CPU = 16000000UL
MEGA_BAUD = 115200

# Default target
all: blink_mega328p.hex blink_mega1284p.hex blink_mega2560.hex

# Arduino Uno
blink_mega328p.elf: blink_mega328p.c
	$(CC) -mmcu=$(UNO_MCU) -DF_CPU=$(UNO_F_CPU) $(CFLAGS) $(LDFLAGS) \
		-I$(TIMER_INCLUDE) -L$(TIMER_LIB_DIR) \
		blink_mega328p.c -ltimer_$(UNO_MCU) -o blink_mega328p.elf
	$(SIZE) blink_mega328p.elf

blink_mega328p.hex: blink_mega328p.elf
	$(OBJCOPY) -O ihex -R .eeprom blink_mega328p.elf blink_mega328p.hex

# ATmega1284P
blink_mega1284p.elf: blink_mega1284p.c
	$(CC) -mmcu=$(M1284_MCU) -DF_CPU=$(M1284_F_CPU) $(CFLAGS) $(LDFLAGS) \
		-I$(TIMER_INCLUDE) -L$(TIMER_LIB_DIR) \
		blink_mega1284p.c -ltimer_$(M1284_MCU) -o blink_mega1284p.elf
	$(SIZE) blink_mega1284p.elf

blink_mega1284p.hex: blink_mega1284p.elf
	$(OBJCOPY) -O ihex -R .eeprom blink_mega1284p.elf blink_mega1284p.hex

# Arduino Mega
blink_mega2560.elf: blink_mega2560.c
	$(CC) -mmcu=$(MEGA_MCU) -DF_CPU=$(MEGA_F_CPU) $(CFLAGS) $(LDFLAGS) \
		-I$(TIMER_INCLUDE) -L$(TIMER_LIB_DIR) \
		blink_mega2560.c -ltimer_$(MEGA_MCU) -o blink_mega2560.elf
	$(SIZE) blink_mega2560.elf

blink_mega2560.hex: blink_mega2560.elf
	$(OBJCOPY) -O ihex -R .eeprom blink_mega2560.elf blink_mega2560.hex

# Clean
clean:
	rm -f *.elf *.hex *.o

# Help
help:
	@echo "Arduino Blink Examples"
	@echo ""
	@echo "Usage:"
	@echo "  make                    - Build all examples"
	@echo "  make blink_mega328p.hex - Build for Arduino Uno"
	@echo "  make blink_mega1284.hex - Build for ATmega1284P"
	@echo "  make blink_mega2560.hex - Build for Arduino Mega"
	@echo "  make clean              - Remove build files"
	@echo ""
	@echo "Adjust ports in Makefile for your system"

.PHONY: all upload-uno upload-1284 upload-mega clean help
