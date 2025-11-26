# AVR ATtiny Fuses Guide

## Overview

Fuses are configuration bits programmed separately from flash memory. They control clock source, startup time, brown-out detection, and programming options. Wrong fuse settings can brick your chip!

## Fuse Types

AVR ATtiny devices have three fuse bytes:
- **Low Fuse (LFUSE)**: Clock selection and startup time
- **High Fuse (HFUSE)**: Reset pin, debug wire, memory protection  
- **Extended Fuse (EFUSE)**: Brown-out detection (BOD)

## Fuse Bit Convention

Fuse bits use inverted logic: 0 = Programmed (enabled), 1 = Unprogrammed (disabled). This is backwards from normal logic!

## Low Fuse - Clock Settings

**CKSEL[3:0]** - Clock Select: Selects clock source (internal RC, external crystal, etc.). BRICK RISK: Wrong external clock setting without hardware present will brick the device.

**SUT[1:0]** - Startup Time: Delay from reset until code runs. Safe to modify.

**CKDIV8** - Divide Clock by 8: When programmed (0), divides system clock by 8. Factory default is programmed. Set to 1 to run at full speed. Safe to modify.

**CKOUT** - Clock Output: Output system clock on a pin. Safe to modify.

## High Fuse - Critical Settings

**RSTDISBL** - Reset Disable: Disables external reset pin (becomes I/O). BRICK RISK: Can't reprogram via ISP without HV programmer. Keep at 1!

**DWEN** - debugWIRE Enable: Enables debugWIRE on reset pin. BRICK RISK: Disables ISP programming. Keep at 1!

**SPIEN** - SPI Enable: Enables SPI programming. CRITICAL BRICK RISK: Must always stay 0 (programmed) or chip becomes unprogrammable.

**WDTON** - Watchdog Always On: Forces watchdog timer always enabled. Annoying but won't brick.

**EESAVE** - EEPROM Save: Preserves EEPROM during chip erase. Safe to modify.

**BODLEVEL[2:0]** - BOD Level: Brown-out detection voltage threshold. Safe to modify.

## Extended Fuse

**SELFPRGEN** - Self Programming: Enables SPM instruction. Safe to modify.

**BODLEVEL[2:0]** - BOD Level: Brown-out voltage threshold (varies by device). Safe to modify.

## Device-Specific Configurations

**ATtiny10**: Only has one fuse byte. Limited to internal oscillator. Lower brick risk.

**ATtiny13**: Internal RC oscillator runs at 9.6MHz (not 8MHz). Factory default: 9.6MHz/8 = 1.2MHz (CKDIV8 programmed). For full speed: LFUSE=0x7A (disables CKDIV8). Safe defaults: LFUSE=0x6A, HFUSE=0xFF, EFUSE=0xFF.

**ATtiny2313**: Has 4MHz or 8MHz internal oscillator. Be careful with external clock settings.

**ATtiny25/45/85**: Internal RC runs at 8MHz. Factory default: 8MHz/8 = 1MHz (CKDIV8 programmed). For full speed: Common settings are LFUSE=0xE2, HFUSE=0xDF, EFUSE=0xFF. Safe defaults: LFUSE=0x62, HFUSE=0xDF, EFUSE=0xFF.

## Critical Warnings - Will Brick Without Special Hardware

1. RSTDISBL = 0 (Reset disabled): Requires high-voltage (HV) programmer to recover
2. DWEN = 0 (debugWire enabled): Requires debugWire tool to disable  
3. SPIEN = 1 (SPI disabled): Chip becomes unprogrammable via ISP
4. Wrong CKSEL with no external clock: Chip expects external clock that isn't there

## Recovery Options

If you brick a device, you'll need a High-Voltage programmer. High-Voltage Parallel Programming (HVPP) works for larger chips. High-Voltage Serial Programming (HVSP) works for 8-pin devices like ATtiny13/25/45/85. If you regularly experiment with fuses, consider building or buying an HV programmer.

## Useful Commands

Read current fuses: `avrdude -c usbasp -p t13`

Write fuses with 0x prefix: `avrdude -c usbasp -p t13 -U lfuse:w:0x7a:m -U hfuse:w:0xff:m -U efuse:w:0xff:m`

Always verify settings before writing. Online fuse calculators like https://www.engbedded.com/fusecalc/ can help visualize settings.