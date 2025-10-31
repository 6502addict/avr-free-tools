# AVR Programmer Script Manual

## Overview

The `avr-prog` script is a unified command-line tool for programming AVR microcontrollers using various hardware programmers. It simplifies the use of avrdude by providing a consistent interface across different programmer types.

## Supported Programmers

- **AVR ISP mkII** - Standard ISP programmer
- **JTAG ICE mkII** - ISP and JTAG debugging
- **STK500** - Development board with ISP and High Voltage programming
- **Atmel-ICE** - Modern programmer with multiple interfaces

## Installation

1. Save the script as `avr-prog`
2. Make it executable:
```bash
   chmod +x avr-prog
```
3. Optionally, move to your PATH:
```bash
   sudo mv avr-prog /usr/local/bin/
```

## Basic Usage
```bash
./avr-prog <programmer> <mode> <device> [options]
```

**Parameters:**
- `<programmer>`: avrisp, jtagice, stk500, or atmelice
- `<mode>`: Operation to perform (program, erase, readfuse, etc.)
- `<device>`: AVR device code (m328p, t85, m2560, etc.)
- `[options]`: Additional parameters depending on mode

## Common Device Codes

| Arduino Board | AVR Chip | Device Code |
|--------------|----------|-------------|
| Arduino Uno | ATmega328P | m328p |
| Arduino Mega 2560 | ATmega2560 | m2560 |
| Arduino Mega 1280 | ATmega1280 | m1280 |
| - | ATmega1284P | m1284p |
| - | ATtiny85 | t85 |
| - | ATtiny45 | t45 |
| - | ATtiny13 | t13 |
| - | ATtiny2313 | t2313 |

## Common Operations

### Programming Flash

Program a hex file to the microcontroller:
```bash
./avr-prog avrisp program m328p firmware.hex
./avr-prog jtagice program m2560 application.hex
./avr-prog stk500 program t85 blink.hex
```

### Erasing Chip

Erase all flash memory:
```bash
./avr-prog avrisp erase m328p
./avr-prog stk500 erase t2313
```

### Reading Fuses

Read the current fuse settings:
```bash
./avr-prog avrisp readfuse m328p
./avr-prog stk500 readfuse m2560
```

Output format: `lfuse hfuse efuse` in hexadecimal

### Writing Fuses

**⚠️ WARNING:** Writing incorrect fuses can brick your chip!
```bash
./avr-prog avrisp writefuse m328p 0xFF 0xDE 0xFD
```

The script will ask for confirmation before writing fuses.

**Common fuse settings:**

**ATmega328P (Arduino Uno) - 16MHz external crystal:**
```bash
./avr-prog avrisp writefuse m328p 0xFF 0xDE 0xFD
```

**ATmega2560 - 16MHz external crystal:**
```bash
./avr-prog avrisp writefuse m2560 0xF7 0xD9 0xFD
```

**ATmega1284P - 16MHz external crystal:**
```bash
./avr-prog avrisp writefuse m1284p 0xF7 0xD9 0xFF
```

**ATtiny85 - 8MHz internal oscillator, no clock division:**
```bash
./avr-prog avrisp writefuse t85 0xE2 0xDF 0xFF
```

**ATtiny13 - 9.6MHz internal oscillator, no clock division:**
```bash
./avr-prog avrisp writefuse t13 0x7A 0xFF 0xFF
```

### Verifying Flash

Verify that the programmed code matches the hex file:
```bash
./avr-prog avrisp verify m328p firmware.hex
```

## Using the AVR ISP mkII

The AVR ISP mkII is the most common programmer for hobbyist use.

### Connection

Connect the 6-pin ISP cable to your target board's ISP header:
```
ISP Header Pinout (top view):
┌─────────────┐
│ MISO  VCC   │  1  2
│ SCK   MOSI  │  3  4
│ RST   GND   │  5  6
└─────────────┘
```

Ensure your target board is powered (either from the ISP or external power).

### Basic Usage
```bash
# Program a chip
./avr-prog avrisp program m328p firmware.hex

# Read fuses
./avr-prog avrisp readfuse m328p

# Erase chip
./avr-prog avrisp erase m328p
```

### Troubleshooting

**"Device signature mismatch":**
- Wrong device code specified
- Check with `avrdude -c avrispmkii -p ? -P usb` for valid codes

**"Initialization failed, rc=-1":**
- Bad connection - check all 6 ISP wires
- Target not powered
- Wrong voltage levels (3.3V vs 5V)
- Try slower clock: script already uses `-B 10`

## Using the JTAG ICE mkII

The JTAG ICE mkII supports both ISP and JTAG debugging modes.

### ISP Mode (Default)

Use like AVR ISP mkII:
```bash
./avr-prog jtagice program m328p firmware.hex
./avr-prog jtagice readfuse m2560
```

### Connection

Same 6-pin ISP header as AVR ISP mkII.

## Using the STK500

The STK500 is a complete development system that requires proper hardware configuration before use.

### Serial Port Configuration

The STK500 connects via RS-232 serial port. By default, the script uses `/dev/ttyUSB0`.

To change the port, edit the script:
```bash
STK500_PORT="/dev/ttyUSB0"  # Change to your port
```

To find your STK500 port:
```bash
# Before plugging in STK500
ls /dev/ttyUSB*

# Plug in STK500
ls /dev/ttyUSB*

# New device that appears is your STK500
```

On Windows with WSL, you may need to use `usbipd` to share the COM port.

### Hardware Setup - CRITICAL

**⚠️ IMPORTANT: The STK500 MUST be configured with jumpers for the specific chip you're programming!**

The STK500 comes with ribbon cables and jumper wires in the kit. **You must manually configure the board for each chip type.** Incorrect configuration will prevent programming or damage the chip.

#### Jumper Configuration Basics

The STK500 has several headers and sockets:
- **SCKT3100A3** - 8-pin socket (for ATtiny)
- **SCKT3100D3** - 28/40-pin socket (for ATmega)
- **ISP6PIN** - ISP signal header
- **PORTB, PORTC, PORTD** - I/O headers
- **VTG** - Target voltage selection
- **AREF** - Analog reference

**Always consult the STK500 User Guide (included with your kit or download from Microchip) for the specific chip you're using!**

#### Example: ATtiny85 (8-pin DIP)

1. **Insert chip** into SCKT3100A3 socket
2. **Connect power jumpers:**
   - Connect VTG to VCC rail (selects 5V or 3.3V)
   - Set voltage jumper to 5V or 3.3V as appropriate
3. **Connect ISP jumpers** from ISP6PIN header to chip socket:
   - RESET → Pin 1 (PB5/RESET)
   - SCK → Pin 7 (PB2/SCK)
   - MISO → Pin 6 (PB1/MISO)
   - MOSI → Pin 5 (PB0/MOSI)
   - VCC → VTG
   - GND → GND
4. **Connect crystal** if using external oscillator (optional):
   - Connect XTAL1 and XTAL2 jumpers
   - Insert crystal into XTAL socket

#### Example: ATmega328P (28-pin DIP)

1. **Insert chip** into SCKT3100D3 socket (use 28-pin section)
2. **Connect power:**
   - VTG jumpers to VCC and GND of chip
   - Set voltage selection (usually 5V)
3. **Connect ISP signals:**
   - Use the ribbon cable to connect ISP6PIN to the socket's ISP pins
   - RESET → Pin 1 (PC6/RESET)
   - SCK → Pin 19 (PB5/SCK)
   - MISO → Pin 18 (PB4/MISO)
   - MOSI → Pin 17 (PB3/MOSI)
4. **Connect oscillator:**
   - If using external crystal, connect XTAL1/XTAL2
   - Insert appropriate crystal (16MHz for Arduino compatibility)

#### Example: ATmega1284P (40-pin DIP)

1. **Insert chip** into SCKT3100D3 socket (40-pin section) or use external connection
2. **Configure jumpers** for 40-pin DIP layout:
   - VCC → Pin 10
   - GND → Pin 11, Pin 31
   - RESET → Pin 9
   - SCK → Pin 8 (PB7)
   - MISO → Pin 7 (PB6)
   - MOSI → Pin 6 (PB5)
3. **Connect power and ISP signals** using jumper wires
4. **Connect oscillator** if using external crystal:
   - XTAL1 → Pin 13 (XTAL1)
   - XTAL2 → Pin 12 (XTAL2)

#### Example: ATtiny2313 (20-pin DIP)

1. **Insert chip** into appropriate socket
2. **Connect ISP signals:**
   - RESET → Pin 1 (PA2/RESET)
   - SCK → Pin 19 (PB7/SCK)
   - MISO → Pin 18 (PB6/MISO)
   - MOSI → Pin 17 (PB5/MOSI)
3. **Connect power**
4. **Connect oscillator** if needed

### Verifying Jumper Configuration

Before programming:

1. **Power on** the STK500
2. **Check LEDs** - the power LED should be on
3. **Verify voltage** at VTG with a multimeter (should match your selection: 3.3V or 5V)
4. **Test connectivity** - use continuity mode to verify jumpers are making good contact

### ISP Programming with STK500

Once jumpers are configured:
```bash
# Program flash
./avr-prog stk500 program m328p firmware.hex

# Read fuses
./avr-prog stk500 readfuse t85

# Erase chip
./avr-prog stk500 erase m2560
```

### High Voltage Programming (Chip Recovery)

The STK500's most powerful feature is High Voltage Programming, which can recover "bricked" chips with incorrect fuse settings.

#### When to Use High Voltage Programming

Use HV programming when:
- RESET pin has been disabled (RSTDISBL fuse programmed)
- Wrong clock source selected and chip won't respond
- Clock fuses set incorrectly (CKSEL bits)
- Any fuse combination that prevents ISP programming

#### High Voltage Serial Programming (HVSP) - For ATtiny

**Setup:**
1. Configure STK500 jumpers for your ATtiny chip (same as ISP mode)
2. Ensure chip is in socket and powered
3. Verify all control signals are connected

**Erase chip and reset fuses (most common operation):**
```bash
./avr-prog stk500 hvsp-erase t85
```

This will:
- Erase all flash memory
- Reset all fuses to factory defaults
- Restore chip to programmable state

**Program via HVSP:**
```bash
./avr-prog stk500 hvsp t85 firmware.hex
```

#### High Voltage Parallel Programming (HVPP) - For ATmega

**Setup:**
1. Configure STK500 jumpers for your ATmega chip
2. Connect all required signals (more complex than ISP - see STK500 manual)
3. Ensure 12V supply is working

**Erase chip and reset fuses:**
```bash
./avr-prog stk500 hvpp-erase m328p
```

**Program via HVPP:**
```bash
./avr-prog stk500 hvpp m328p firmware.hex
```

#### High Voltage Programming Notes

- **12V supply required:** STK500 generates this internally
- **More connections:** HV modes use more pins than ISP
- **Slower:** HV programming is slower than ISP
- **Last resort:** Use only when ISP fails
- **Consult manual:** HV mode pin configurations are chip-specific

### STK500 Troubleshooting

**"Programmer not responding":**
- Check serial port setting in script
- Try different USB-to-serial adapter
- Check STK500 power supply
- Update STK500 firmware if needed

**"Verification error":**
- Incorrect jumpers - verify all ISP signals connected
- Bad chip socket contact - reseat chip
- Wrong voltage selection - check VTG jumper

**"Device signature = 0x000000" or "0xFFFFFF":**
- Chip not powered - check VTG jumpers
- Missing clock - check XTAL jumpers if using external crystal
- Bad ISP connections - verify all 6 signals

## Using the Atmel-ICE

The Atmel-ICE is a modern programmer supporting multiple programming interfaces.

### ISP Mode

Standard ISP programming (same as AVR ISP mkII):
```bash
./avr-prog atmelice program m328p firmware.hex
./avr-prog atmelice readfuse m2560
```

### JTAG Mode

For chips with JTAG interface (ATmega128, ATmega2560, etc.):
```bash
./avr-prog atmelice jtag m2560 firmware.hex
```

### debugWIRE Mode

For ATtiny and some ATmega chips with single-wire debug:
```bash
./avr-prog atmelice debugwire t85 firmware.hex
```

**Note:** debugWIRE uses the RESET pin and requires DWEN fuse to be enabled.

### PDI Mode (XMEGA)

For XMEGA series chips:
```bash
./avr-prog atmelice pdi x128a1 firmware.hex
```

### UPDI Mode (New AVR)

For modern AVR chips (AVR-DA, AVR-DB, AVR-DD, etc.):
```bash
./avr-prog atmelice updi avr128da28 firmware.hex
./avr-prog atmelice updi avr64db32 application.hex
```

## Complete Command Reference

### All Programmers - Common Operations
```bash
# Programming
./avr-prog <programmer> program <device> <hexfile>

# Erase
./avr-prog <programmer> erase <device>

# Read fuses
./avr-prog <programmer> readfuse <device>

# Write fuses
./avr-prog <programmer> writefuse <device> <lfuse> <hfuse> <efuse>

# Verify
./avr-prog <programmer> verify <device> <hexfile>
```

### STK500 - High Voltage Operations
```bash
# HVSP programming (ATtiny)
./avr-prog stk500 hvsp <device> <hexfile>
./avr-prog stk500 hvsp-erase <device>

# HVPP programming (ATmega)
./avr-prog stk500 hvpp <device> <hexfile>
./avr-prog stk500 hvpp-erase <device>
```

### Atmel-ICE - Special Interfaces
```bash
# JTAG
./avr-prog atmelice jtag <device> <hexfile>

# debugWIRE
./avr-prog atmelice debugwire <device> <hexfile>

# PDI (XMEGA)
./avr-prog atmelice pdi <device> <hexfile>

# UPDI (new AVR)
./avr-prog atmelice updi <device> <hexfile>
```

## Examples

### Example 1: Program Arduino Uno
```bash
# Compile your code first
avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -Os main.c -o main.elf
avr-objcopy -O ihex main.elf main.hex

# Program with AVR ISP mkII
./avr-prog avrisp program m328p main.hex
```

### Example 2: Set Fuses for 16MHz External Crystal
```bash
# ATmega328P
./avr-prog avrisp writefuse m328p 0xFF 0xDE 0xFD
```

### Example 3: Recover Bricked ATtiny85
```bash
# Chip has RSTDISBL fuse set and won't respond to ISP
# Use STK500 with HVSP to recover

# 1. Configure STK500 jumpers for ATtiny85
# 2. Run HVSP erase (resets fuses to defaults)
./avr-prog stk500 hvsp-erase t85

# 3. Verify chip is recovered
./avr-prog stk500 readfuse t85

# 4. Program normally
./avr-prog stk500 program t85 firmware.hex
```

### Example 4: Program ATmega1284P
```bash
# Read current fuses
./avr-prog avrisp readfuse m1284p

# Program application
./avr-prog avrisp program m1284p application.hex

# Verify programming
./avr-prog avrisp verify m1284p application.hex
```

### Example 5: Work with New AVR (UPDI)
```bash
# Program AVR128DA28 using Atmel-ICE
./avr-prog atmelice updi avr128da28 firmware.hex
```

## Troubleshooting

### General Issues

**"avrdude: command not found":**
- Install avrdude: `sudo apt-get install avrdude` (Linux) or download from microchip.com

**"Permission denied" on /dev/ttyUSB0:**
```bash
sudo usermod -a -G dialout $USER
# Log out and log back in
```

**"Device signature mismatch":**
- Wrong device code specified
- Chip is damaged or not properly connected
- Wrong voltage (3.3V chip with 5V programmer)

**"Initialization failed, rc=-1":**
- Check all connections
- Verify target is powered
- Try slower programming speed (already set with -B 10)
- Check for shorts or bad solder joints

### Programmer-Specific Issues

**AVR ISP mkII / JTAG ICE mkII:**
- Update firmware if needed (use Atmel Studio)
- Check USB cable
- Try different USB port

**STK500:**
- Wrong serial port - check connection
- Jumpers not configured - see Hardware Setup section
- Firmware outdated - update via Atmel Studio
- Check that crystal is installed if using external clock

**Atmel-ICE:**
- Select correct interface mode
- Check that target supports selected interface (JTAG/debugWIRE/UPDI)
- Verify connections match interface requirements

## Fuse Calculator

Understanding fuses is critical. Use online fuse calculators:
- https://www.engbedded.com/fusecalc/
- Select your chip
- Choose desired settings
- Get hex values for lfuse, hfuse, efuse

**Always double-check fuse values before writing!**

## Safety Notes

1. **Fuses can brick chips** - Always verify fuse values before writing
2. **Voltage matching** - Use 3.3V programmer for 3.3V chips, 5V for 5V chips
3. **Polarity matters** - ISP connector has a notch/key, match it
4. **Power before connect** - Power target before connecting programmer
5. **Backup important fuses** - Record original fuse values: `./avr-prog <prog> readfuse <device>`

## Additional Resources

- AVR Instruction Set Manual: https://www.microchip.com/
- AVR Fuse Calculator: https://www.engbedded.com/fusecalc/
- avrdude Documentation: https://www.nongnu.org/avrdude/
- STK500 User Guide: Available from Microchip website
- ATmega datasheets: https://www.microchip.com/

## License

This script and documentation are provided as-is for educational and development purposes.

## Version

Version 1.0 - 2025

---

**Remember: When in doubt, consult the datasheet for your specific AVR chip and the user manual for your programmer!**