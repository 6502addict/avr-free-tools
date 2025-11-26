# Project build template

# Default MCU list - can be overridden
MCUS ?= atmega328p atmega1284 atmega1284p atmega2560 attiny13 attiny25 attiny45 attiny85 attiny2313

# Set F_CPU based on MCU if not provided
ifeq ($(MCU),atmega328p)
    F_CPU ?= 16000000UL
else ifeq ($(MCU),atmega1284)
    F_CPU ?= 16000000UL
else ifeq ($(MCU),atmega1284p)
    F_CPU ?= 16000000UL
else ifeq ($(MCU),atmega2560)
    F_CPU ?= 16000000UL
else ifeq ($(MCU),attiny13)
    F_CPU ?= 9600000UL
else ifeq ($(MCU),attiny25)
    F_CPU ?= 8000000UL
else ifeq ($(MCU),attiny45)
    F_CPU ?= 8000000UL
else ifeq ($(MCU),attiny85)
    F_CPU ?= 8000000UL
else ifeq ($(MCU),attiny2313)
    F_CPU ?= 8000000UL
else
    F_CPU ?= 16000000UL
endif

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET).hex size

# Compile
%.o: %.c
	$(CC) $(CFLAGS) -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I$(INCLUDE_DIR) -I. -c $< -o $@

# Link
$(TARGET).elf: $(OBJ)
	$(CC) -mmcu=$(MCU) $(OBJ) -L$(LIB_DIR) $(LIBS) -o $@

# Create hex file
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Show size
size: $(TARGET).elf
	$(SIZE) --format=avr --mcu=$(MCU) $<

# Build for all MCUs
all-mcus: clean
	@for mcu in $(MCUS); do \
		echo "Building for $$mcu..."; \
		case $$mcu in \
			atmega328p|atmega1284|atmega1284p|atmega2560) fcpu=16000000UL;; \
			attiny13) fcpu=9600000UL;; \
			attiny2313|attiny25|attiny45|attiny85) fcpu=8000000UL;; \
			*) fcpu=16000000UL;; \
		esac; \
		$(MAKE) MCU=$$mcu F_CPU=$$fcpu || exit 1; \
		mv $(TARGET).hex $(TARGET)_$$mcu.hex; \
		mv $(TARGET).elf $(TARGET)_$$mcu.elf; \
		$(MAKE) clean; \
		echo ""; \
	done

clean:
	rm -f $(TARGET).o $(TARGET).elf $(TARGET).hex *~

.PHONY: all all-mcus size clean

