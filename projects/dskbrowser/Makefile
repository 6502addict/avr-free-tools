include ../common.mk

TARGET = dskbrowser
SRC = $(TARGET).c
MCUS = atmega1284p atmega2560
LIBS = -lfatfs_$(MCU)  -lsdcard_$(MCU) -lspi_$(MCU) -ltimer_$(MCU) 

ifneq ($(findstring tiny,$(MCU)),)
    LIBS += -luart-tiny_$(MCU)
else
    LIBS += -luart-mega_$(MCU)
endif

include ../project.mk
