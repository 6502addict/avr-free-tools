MCU = attiny45
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
BAUD = 1200
TARGET = calibrate

# Chip-specific settings
ifeq ($(MCU),attiny13)
    F_CPU = 9600000UL
endif

ifeq ($(MCU),attiny25)
    F_CPU = 8200000UL
endif

ifeq ($(MCU),attiny45)
    F_CPU = 8200000UL
endif

ifeq ($(MCU),attiny85)
    F_CPU = 8200000UL
endif

ifeq ($(MCU),attiny2313)
    F_CPU = 8200000UL
endif


CFLAGS = -mmcu=$(MCU) -D$(MCU) -DBAUD_RATE=$(BAUD) -DF_CPU=$(F_CPU) -Os -Wall -Wl,--relax

all: $(TARGET).hex size

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

$(TARGET).elf: $(TARGET).o uart_tiny.o
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET).o: $(TARGET).c uart_tiny.h
	$(CC) $(CFLAGS) -c $(TARGET).c

uart_tiny.o: uart_tiny.c uart_tiny.h
	$(CC) $(CFLAGS) -c uart_tiny.c

size: $(TARGET).elf
	@echo "Flash and RAM usage:"
	$(SIZE) -C --mcu=$(MCU) $<

# Clean
clean:
	@echo "Cleaning..."
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).o uart_tiny.o

all-mcus:
	@echo "Building for all chips..."
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
	@make clean
	@make MCU=attiny2313
	@mv $(TARGET).hex $(TARGET)_t2313.hex
	@make clean
	@echo "Done! Created: $(TARGET)_t13.hex, $(TARGET)_t25.hex, $(TARGET)_t45.hex, $(TARGET)_t45.hex, $(TARGET)_t45.hex"
