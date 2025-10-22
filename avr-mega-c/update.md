# Adding New AVR Chips to the Makefile

## Quick Reference

To add a new chip, you need to add one `ifeq` block with 3-4 settings.

## Step-by-Step Guide

### 1. Find the chip-specific settings section

Look for this section in the Makefile:

```makefile
# Chip-specific settings
ifeq ($(MCU),attiny85)
    F_CPU = 8000000UL
    PROGRAMMER = usbtiny
    BAUD = 9600
endif
```

### 2. Add your new chip block

Add a new `ifeq` block following this template:

```makefile
ifeq ($(MCU),your_chip_name)
    F_CPU = clock_speed
    PROGRAMMER = programmer_type
    BAUD = baud_rate
    PROGRAMMER_BAUD = programmer_baud  # Optional, only if needed
endif
```

### 3. Required settings explained

**MCU name**: Use the exact name as recognized by avr-gcc
- Examples: `atmega328p`, `atmega2560`, `atmega1284p`, `attiny85`
- Find valid names: `avr-gcc --target-help | grep mcpu`

**F_CPU**: Clock frequency in Hz
- Common values: `8000000UL` (8MHz), `16000000UL` (16MHz)
- Must match your actual crystal/oscillator
- The `UL` suffix means "unsigned long"

**PROGRAMMER**: Programmer type for avrdude
- `arduino` - Arduino bootloader (ATmega328P on Uno)
- `wiring` - Arduino Mega bootloader (ATmega2560)
- `usbasp` - USBasp programmer
- `usbtiny` - USBtinyISP programmer
- Find all types: `avrdude -c ?`

**BAUD**: Serial communication baud rate
- Common values: `9600`, `19200`, `38400`, `57600`, `115200`
- Lower baud rates work better with non-standard clock speeds
- If you get setbaud.h warnings, try a lower baud rate

**PROGRAMMER_BAUD**: (Optional) Programmer communication speed
- Only needed for some programmers like `wiring`
- Example: `115200` for Arduino Mega bootloader

## Examples

### Example 1: ATmega328P (already included)

```makefile
ifeq ($(MCU),atmega328p)
    F_CPU = 16000000UL
    PROGRAMMER = arduino
    BAUD = 115200
endif
```

### Example 2: ATmega8

```makefile
ifeq ($(MCU),atmega8)
    F_CPU = 16000000UL
    PROGRAMMER = usbasp
    BAUD = 57600
endif
```

### Example 3: ATtiny2313

```makefile
ifeq ($(MCU),attiny2313)
    F_CPU = 8000000UL
    PROGRAMMER = usbtiny
    BAUD = 9600
endif
```

### Example 4: ATmega32U4 (Arduino Leonardo)

```makefile
ifeq ($(MCU),atmega32u4)
    F_CPU = 16000000UL
    PROGRAMMER = avr109
    BAUD = 115200
endif
```

## Common Baud Rates by Clock Speed

### 8 MHz
- Safe: `9600`, `19200`, `38400`
- Risky: `57600` (check setbaud.h warnings)

### 16 MHz
- Safe: `9600`, `19200`, `38400`, `57600`, `115200`
- Note: ATmega1284P may warn at 115200, use 57600

### 20 MHz
- Safe: All standard rates work well

## Testing Your New Chip

1. Add the chip configuration to the Makefile
2. Build: `make MCU=your_chip_name`
3. Check for warnings about baud rate
4. If you see setbaud.h warnings, lower the BAUD setting
5. Upload: `make MCU=your_chip_name upload`

## Troubleshooting

### Warning: "Baud rate achieved is higher than allowed"

**Problem**: The baud rate can't be accurately achieved with the clock speed

**Solution**: Lower the BAUD value (try 57600 instead of 115200)

### Error: unknown MCU

**Problem**: avr-gcc doesn't recognize the MCU name

**Solution**: Check valid names with `avr-gcc --target-help | grep mcpu`

### Upload fails

**Problem**: Wrong programmer type or port

**Solution**: 
- Check programmer type: `avrdude -c ?`
- Check port: `ls /dev/ttyUSB*` or `ls /dev/ttyACM*`
- Update AVRDUDE_FLAGS if using different port

## Quick Copy Template

```makefile
ifeq ($(MCU),NEW_CHIP_HERE)
    F_CPU = 16000000UL
    PROGRAMMER = usbasp
    BAUD = 57600
endif
```

Replace `NEW_CHIP_HERE` with your chip name and adjust the values as needed!