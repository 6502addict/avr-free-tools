# AVR Common settings
CC = avr-gcc
AR = avr-ar
OBJCOPY = avr-objcopy
SIZE = avr-size

BAUD ?= 9600

# Project root paths - common.mk is in projects/, root is one level up
#PROJECT_ROOT = $(abspath $(dir $(lastword $(MAKEFILE_LIST)))..)
PROJECT_ROOT = $(abspath ../..)
LIB_DIR = $(PROJECT_ROOT)/lib
INCLUDE_DIR = $(PROJECT_ROOT)/include

# Base compiler flags
CFLAGS = -Os -Wall -Wextra -std=gnu99
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections
