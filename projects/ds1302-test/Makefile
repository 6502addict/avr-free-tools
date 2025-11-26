include ../common.mk

TARGET = ds1302-test
SRC = $(TARGET).c
MCUS = atmega328p atmega1284 atmega1284p atmega2560
LIBS = -lds1302_$(MCU) 

ifneq ($(findstring tiny,$(MCU)),)
    LIBS += -luart-tiny_$(MCU)
else
    LIBS += -luart-mega_$(MCU)
endif

include ../project.mk
