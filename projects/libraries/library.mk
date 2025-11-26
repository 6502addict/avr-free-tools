# Library build template

PROJECT_ROOT = $(abspath ../../..)
LIB_DIR = $(PROJECT_ROOT)/lib
INCLUDE_DIR = $(PROJECT_ROOT)/include

BUILD_DIR = build

MCUS ?= atmega328p atmega1284 atmega1284p atmega2560 attiny13 attiny25 attiny45 attiny85


# Build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Pattern rule for object files
$(BUILD_DIR)/%_atmega328p.o: %.c %.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -mmcu=atmega328p -DF_CPU=16000000UL -DBAUD=$(BAUD) -I. -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/%_atmega1284.o: %.c %.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -mmcu=atmega1284 -DF_CPU=16000000UL -DBAUD=$(BAUD) -I. -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/%_atmega1284p.o: %.c %.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -mmcu=atmega1284p -DF_CPU=16000000UL -DBAUD=$(BAUD) -I. -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/%_atmega2560.o: %.c %.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -mmcu=atmega2560 -DF_CPU=16000000UL -DBAUD=$(BAUD) -I. -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/%_attiny13.o: %.c %.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -mmcu=attiny13 -DF_CPU=9600000UL -DBAUD=$(BAUD) -I. -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/%_attiny25.o: %.c %.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -mmcu=attiny25 -DF_CPU=8000000UL -DBAUD=$(BAUD) -I. -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/%_attiny45.o: %.c %.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -mmcu=attiny45 -DF_CPU=8000000UL -DBAUD=$(BAUD) -I. -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/%_attiny85.o: %.c %.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -mmcu=attiny85 -DF_CPU=8000000UL -DBAUD=$(BAUD) -I. -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/%_attiny2313.o: %.c %.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -mmcu=attiny85 -DF_CPU=8000000UL -DBAUD=$(BAUD) -I. -I$(INCLUDE_DIR) -c $< -o $@

# Pattern rule for libraries
$(BUILD_DIR)/lib%_atmega328p.a: $(BUILD_DIR)/%_atmega328p.o
	$(AR) rcs $@ $^
	$(SIZE) $@

$(BUILD_DIR)/lib%_atmega1284.a: $(BUILD_DIR)/%_atmega1284.o
	$(AR) rcs $@ $^
	$(SIZE) $@

$(BUILD_DIR)/lib%_atmega1284p.a: $(BUILD_DIR)/%_atmega1284p.o
	$(AR) rcs $@ $^
	$(SIZE) $@

$(BUILD_DIR)/lib%_atmega2560.a: $(BUILD_DIR)/%_atmega2560.o
	$(AR) rcs $@ $^
	$(SIZE) $@

$(BUILD_DIR)/lib%_attiny13.a: $(BUILD_DIR)/%_attiny13.o
	$(AR) rcs $@ $^
	$(SIZE) $@

$(BUILD_DIR)/lib%_attiny25.a: $(BUILD_DIR)/%_attiny25.o
	$(AR) rcs $@ $^
	$(SIZE) $@

$(BUILD_DIR)/lib%_attiny45.a: $(BUILD_DIR)/%_attiny45.o
	$(AR) rcs $@ $^
	$(SIZE) $@

$(BUILD_DIR)/lib%_attiny85.a: $(BUILD_DIR)/%_attiny85.o
	$(AR) rcs $@ $^
	$(SIZE) $@

$(BUILD_DIR)/lib%_attiny2313.a: $(BUILD_DIR)/%_attiny2313.o
	$(AR) rcs $@ $^
	$(SIZE) $@

# Default single MCU build
MCU ?= atmega1284p
LIB = $(BUILD_DIR)/lib$(TARGET)_$(MCU).a

all: $(LIB)

# Build all
all-mcus: \
	$(foreach mcu,$(MCUS),$(BUILD_DIR)/lib$(TARGET)_$(mcu).a)

# Install
install: $(LIB) $(TARGET).h
	install -d $(INCLUDE_DIR)
	install -d $(LIB_DIR)
	install -m 644 *.h $(INCLUDE_DIR)/
	install -m 644 $(LIB) $(LIB_DIR)/

install-all: all-mcus $(TARGET).h
	install -d $(INCLUDE_DIR)
	install -d $(LIB_DIR)
	install -m 644 *.h $(INCLUDE_DIR)/
	install -m 644 $(BUILD_DIR)/lib$(TARGET)_*.a $(LIB_DIR)/

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all all-mcus clean install install-all
