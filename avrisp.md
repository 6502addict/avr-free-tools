# AVR Programming Script

## Overview
Simple bash script to program AVR microcontrollers using AVR ISP mkII programmer.

## Script
```bash
#!/bin/bash
avrdude -c avrispmkii -p $1 -P usb -U flash:w:$2:i
```

## Usage
```bash
./script.sh <device> <hexfile>
```

**Example:**
```bash
./script.sh m328p myprogram.hex
./script.sh m2560 myprogram.hex
```

## Parameters

### $1 - Device Type
Common Arduino AVR devices:

| Arduino Board | Device Parameter | Chip |
|--------------|------------------|------|
| Arduino Uno | `m328p` | ATmega328P |
| Arduino Nano | `m328p` | ATmega328P |
| Arduino Mega 2560 | `m2560` | ATmega2560 |
| Arduino Mega 1280 | `m1280` | ATmega1280 |
| Arduino Leonardo | `m32u4` | ATmega32U4 |
| Arduino Micro | `m32u4` | ATmega32U4 |
| Arduino Mini | `m328p` | ATmega328P |

Other common AVR devices:
- `m8` - ATmega8
- `m16` - ATmega16
- `m32` - ATmega32
- `m64` - ATmega64
- `m128` - ATmega128
- `m168` - ATmega168
- `m644p` - ATmega644P
- `m1284p` - ATmega1284P
- `t85` - ATtiny85
- `t84` - ATtiny84
- `t44` - ATtiny44

### $2 - Hex File
Path to your compiled .hex file

## How It Works

1. **#!/bin/bash** - Shebang, tells system to use bash
2. **avrdude** - AVR programming tool
3. **-c avrispmkii** - Programmer type (AVR ISP mkII)
4. **-p $1** - Target device (first command line argument)
5. **-P usb** - Connection port (USB)
6. **-U flash:w:$2:i** - Memory operation:
   - `flash` - target flash memory
   - `w` - write operation
   - `$2` - filename (second argument)
   - `i` - Intel hex format

## Full Device List
Run this command to see all supported devices:
```bash
avrdude -c avrispmkii -p ?
```
