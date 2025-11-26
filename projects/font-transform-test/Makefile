include ../common.mk

TARGET = font-transform-test
SRC = $(TARGET).c
MCUS = atmega328p atmega1284 atmega1284p atmega2560
LIBS = -lfont-transform_$(MCU) -lssd1306_$(MCU) -li2c_$(MCU) 

ifneq ($(findstring tiny,$(MCU)),)
    LIBS += -luart-tiny_$(MCU)
else
    LIBS += -luart-mega_$(MCU)
endif

include ../project.mk
