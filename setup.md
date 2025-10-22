# AVR Development Environment Setup Guide
## For Debian Linux and WSL2

This guide will walk you through installing and configuring all necessary tools to develop Assembly and C programs for AVR Tiny/Mega microcontrollers on Debian Linux or WSL2.

---

## Table of Contents
- [Prerequisites](#prerequisites)
- [Installing AVR Toolchain](#installing-avr-toolchain)
- [Installing Programming Tools](#installing-programming-tools)
- [Installing Debugging Tools](#installing-debugging-tools)
- [USB Device Access (WSL2 Specific)](#usb-device-access-wsl2-specific)
- [Verifying Installation](#verifying-installation)
- [Project Structure](#project-structure)
- [Example Workflow](#example-workflow)
- [Troubleshooting](#troubleshooting)

---

## Prerequisites

Update your system package list:

```bash
sudo apt update
sudo apt upgrade -y
```

Install essential build tools:

```bash
sudo apt install -y build-essential git wget curl
```

---

## Installing AVR Toolchain

### 1. Install AVR GCC Compiler and Binutils

```bash
sudo apt install -y gcc-avr avr-libc binutils-avr
```

**What this installs:**
- `gcc-avr`: GCC compiler for AVR microcontrollers
- `avr-libc`: Standard C library for AVR
- `binutils-avr`: Binary utilities (assembler, linker, etc.)

### 2. Verify Installation

```bash
avr-gcc --version
avr-as --version
avr-ld --version
```

You should see version information for each tool.

---

## Installing Programming Tools

### 1. Install AVRDUDE (AVR Downloader/UploaDEr)

```bash
sudo apt install -y avrdude
```

### 2. Verify AVRDUDE Installation

```bash
avrdude -v
```

### 3. Configure AVRDUDE Permissions

Add your user to the `dialout` group for serial port access:

```bash
sudo usermod -a -G dialout $USER
```

**Important:** Log out and log back in for group changes to take effect.

### 4. Create AVRDUDE Configuration (Optional)

You can create a local `.avrduderc` file in your home directory for custom settings:

```bash
touch ~/.avrduderc
```

---

## Installing Debugging Tools

### 1. Install AVR GDB

```bash
sudo apt install -y gdb-avr
```

### 2. Install AVaRICE (AVR JTAG ICE Interface)

```bash
sudo apt install -y avarice
```

**What is AVaRICE?**
AVaRICE provides a bridge between GDB and JTAG ICE debugging hardware, allowing you to debug AVR programs in real-time.

### 3. Verify Debugging Tools

```bash
avr-gdb --version
avarice --version
```

---

## USB Device Access (WSL2 Specific)

WSL2 doesn't have direct USB access by default. You need to set up USB/IP forwarding.

### 1. Install USBIPD on Windows

Download and install from: https://github.com/dorssel/usbipd-win/releases

### 2. Install USB/IP Tools in WSL2

```bash
sudo apt install -y linux-tools-generic hwdata
sudo update-alternatives --install /usr/local/bin/usbip usbip /usr/lib/linux-tools/*-generic/usbip 20
```

### 3. Attach USB Device

In Windows PowerShell (as Administrator):

```powershell
# List USB devices
usbipd list

# Bind your AVR programmer (replace BUSID with actual value)
usbipd bind --busid <BUSID>

# Attach to WSL
usbipd attach --wsl --busid <BUSID>
```

### 4. Verify USB Device in WSL2

```bash
lsusb
```

You should see your AVR programmer listed.

### 5. Set Up udev Rules

Create a udev rules file for AVR programmers:

```bash
sudo nano /etc/udev/rules.d/99-avr.rules
```

Add the following (adjust VID/PID for your programmer):

```
# USBasp
SUBSYSTEM=="usb", ATTR{idVendor}=="16c0", ATTR{idProduct}=="05dc", GROUP="dialout", MODE="0666"

# Arduino as ISP
SUBSYSTEM=="usb", ATTR{idVendor}=="2341", GROUP="dialout", MODE="0666"

# Atmel JTAG ICE mkII
SUBSYSTEM=="usb", ATTR{idVendor}=="03eb", ATTR{idProduct}=="2103", GROUP="dialout", MODE="0666"
```

Reload udev rules:

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

---

## Verifying Installation

### Complete Verification Script

Create a test script:

```bash
cat > verify_avr_setup.sh << 'EOF'
#!/bin/bash

echo "=== AVR Development Environment Verification ==="
echo ""

echo "1. Checking AVR GCC..."
avr-gcc --version | head -n 1

echo ""
echo "2. Checking AVR Binutils..."
avr-as --version | head -n 1

echo ""
echo "3. Checking AVR Libc..."
dpkg -l | grep avr-libc

echo ""
echo "4. Checking AVRDUDE..."
avrdude -v 2>&1 | head -n 1

echo ""
echo "5. Checking AVR GDB..."
avr-gdb --version | head -n 1

echo ""
echo "6. Checking AVaRICE..."
avarice --version 2>&1 | head -n 1

echo ""
echo "7. Checking user groups..."
groups | grep dialout && echo "✓ User in dialout group" || echo "✗ User NOT in dialout group"

echo ""
echo "8. Checking USB devices..."
lsusb

echo ""
echo "=== Verification Complete ==="
EOF

chmod +x verify_avr_setup.sh
./verify_avr_setup.sh
```

---

## Project Structure

Recommended project structure:

```
my-avr-project/
├── src/
│   ├── main.c
│   └── main.S          # Assembly files
├── include/
│   └── config.h
├── build/
├── Makefile
└── README.md
```

---

## Example Workflow

### 1. Create a Simple Blink Program

```bash
mkdir -p ~/avr-projects/blink/src
cd ~/avr-projects/blink
```

Create `src/main.c`:

```c
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // Set PB5 as output (LED on Arduino Uno)
    DDRB |= (1 << PB5);
    
    while(1) {
        PORTB ^= (1 << PB5);  // Toggle LED
        _delay_ms(1000);       // Wait 1 second
    }
    
    return 0;
}
```

### 2. Create Makefile

Create `Makefile`:

```makefile
# MCU Configuration
MCU = atmega328p
F_CPU = 16000000UL
PROGRAMMER = arduino
PORT = /dev/ttyUSB0
BAUD = 115200

# Compiler and Tools
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
AVRDUDE = avrdude

# Compiler Flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra
LDFLAGS = -mmcu=$(MCU)

# Project Files
TARGET = main
SRC = src/main.c
OBJ = build/main.o
HEX = build/$(TARGET).hex
ELF = build/$(TARGET).elf

# Default target
all: build $(HEX)

# Create build directory
build:
	mkdir -p build

# Compile
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Link
$(ELF): $(OBJ)
	$(CC) $(LDFLAGS) $< -o $@

# Create hex file
$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@
	$(SIZE) $<

# Upload to microcontroller
flash: $(HEX)
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$<:i

# Clean build files
clean:
	rm -rf build

# Phony targets
.PHONY: all clean flash build
```

### 3. Build and Flash

```bash
# Compile the program
make

# Flash to AVR (adjust PORT in Makefile)
make flash
```

### 4. Assembly Example

Create `src/blink.S`:

```assembly
.include "avr/io.h"

.section .text
.global main

main:
    ; Set PB5 as output
    ldi r16, (1 << PB5)
    out _SFR_IO_ADDR(DDRB), r16

loop:
    ; Toggle PB5
    in r16, _SFR_IO_ADDR(PORTB)
    ldi r17, (1 << PB5)
    eor r16, r17
    out _SFR_IO_ADDR(PORTB), r16
    
    ; Delay
    call delay
    
    rjmp loop

delay:
    ; Simple delay loop
    ldi r18, 255
delay_outer:
    ldi r19, 255
delay_inner:
    dec r19
    brne delay_inner
    dec r18
    brne delay_outer
    ret
```

To compile assembly:

```bash
avr-gcc -mmcu=atmega328p -c src/blink.S -o build/blink.o
avr-gcc -mmcu=atmega328p build/blink.o -o build/blink.elf
avr-objcopy -O ihex build/blink.elf build/blink.hex
```

---

## Troubleshooting

### Permission Denied on /dev/ttyUSB0

```bash
sudo chmod 666 /dev/ttyUSB0
# Or add user to dialout group (recommended)
sudo usermod -a -G dialout $USER
```

### AVRDUDE: Device Not Found

1. Check USB connection: `lsusb`
2. Verify correct PORT in Makefile
3. Check programmer type matches your hardware
4. For WSL2, ensure USB device is attached via usbipd

### WSL2 USB Not Working

1. Ensure usbipd-win is installed on Windows
2. Device must be bound before attaching
3. Reattach device after WSL restart:

```powershell
usbipd attach --wsl --busid <BUSID>
```

### GDB Debugging Not Working

1. Ensure AVaRICE is running:
```bash
avarice --jtag /dev/ttyUSB0 :4242
```

2. Connect GDB:
```bash
avr-gdb build/main.elf
(gdb) target remote localhost:4242
```

### Wrong MCU Type

Always verify your MCU type:
- ATmega328P (Arduino Uno)
- ATtiny85
- ATmega2560 (Arduino Mega)

Update in Makefile and AVRDUDE commands accordingly.

---

## Additional Resources

- **AVR Libc Documentation**: https://www.nongnu.org/avr-libc/
- **AVRDUDE Documentation**: https://www.nongnu.org/avrdude/
- **AVR Instruction Set**: https://ww1.microchip.com/downloads/en/DeviceDoc/AVR-InstructionSet-Manual-DS40002198.pdf
- **Atmel Datasheets**: https://www.microchip.com/en-us/products/microcontrollers-and-microprocessors/8-bit-mcus/avr-mcus

---

## Summary of Installed Tools

| Tool | Purpose | Command |
|------|---------|---------|
| avr-gcc | C/C++ compiler | `avr-gcc` |
| avr-as | Assembler | `avr-as` |
| avr-ld | Linker | `avr-ld` |
| avr-objcopy | Object file converter | `avr-objcopy` |
| avrdude | Flash programmer | `avrdude` |
| avr-gdb | Debugger | `avr-gdb` |
| avarice | JTAG interface | `avarice` |

---

**Your AVR development environment is now ready!** Start coding and happy hacking! 🚀