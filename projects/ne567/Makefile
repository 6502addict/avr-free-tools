MCU = atmega328p
F_CPU = 16000000UL
BAUD = 9600

TARGET = ne567
SOURCES = ne567.c uart_console.c

CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -Os -Wall
LDFLAGS = -mmcu=$(MCU)

OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET).hex

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -U flash:w:$

clean:
	rm -f $(OBJECTS) $(TARGET).elf $(TARGET).hex

.PHONY: all flash clean

