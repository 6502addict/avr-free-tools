# ATtiny Internal Clock Calibration Guide

Complete procedure to calibrate the internal RC oscillator of ATtiny13/25/45/85 microcontrollers for accurate UART timing.

## Overview

The ATtiny internal oscillator can vary significantly between chips and with temperature/voltage changes. This guide explains how to calibrate the oscillator frequency to 8MHz using the OSCCAL register for reliable UART communication.

**Target frequency:** 8.0MHz (±2.5% tolerance for UART)  
**Valid OSCCAL range:** Typically 0x56 to 0x65 (varies per chip)  
**Calibrated value storage:** EEPROM byte 0

---

## Prerequisites

**Hardware needed:**
- ATtiny13/25/45/85 chip
- ISP programmer (USBasp, etc.)
- Oscilloscope or frequency counter
- USB-to-serial adapter (1200 baud, 8N1)

**Software needed:**
- avrdude for programming
- calibrate_txx.hex (where xx = 13, 25, 45, or 85)
- Serial terminal (1200 baud, 8N1)

---

## Step 1: Configure Fuses

Before calibration, you must set the correct fuse values to enable clock output and preserve EEPROM.

### Required Fuse Settings

**Low Fuse (lfuse):**
- **CKOUT = 0** (enable clock output on PB4)
- **CKDIV8 = 1** (disable /8 divider for 8MHz operation)
- **Value: 0xA2** (for ATtiny25/45/85)
- **Value: 0x2A** (for ATtiny13)

**High Fuse (hfuse):**
- **EESAVE = 0** (preserve EEPROM during chip erase)
- **Value: 0xD7** (for ATtiny25/45/85)
- **Value: 0xF7** (for ATtiny13)

### Programming the Fuses

```bash
# ATtiny13
avrdude -c usbasp -p t13 -U lfuse:w:0x2A:m -U hfuse:w:0xF7:m

# ATtiny25/45/85
avrdude -c usbasp -p t85 -U lfuse:w:0xA2:m -U hfuse:w:0xD7:m
```

**⚠️ CRITICAL:** Never set EESAVE back to 1 after calibration, or you will lose the calibrated OSCCAL value!

### Verify Fuse Settings

```bash
# Read current fuses
avrdude -c usbasp -p t85 -U lfuse:r:-:h -U hfuse:r:-:h
```

You can also use an online fuse calculator: https://www.engbedded.com/fusecalc/

---

## Step 2: Coarse Calibration with Oscilloscope

With CKOUT enabled, the chip outputs **clock/4** on PB4. For 8MHz internal clock, you should measure **2MHz** on PB4.

### Setup

1. **Connect oscilloscope:**
   - GND → chip GND
   - Probe → PB4 (pin 3 on DIP-8)

2. **Power the chip** (3.3V or 5V)

3. **Measure frequency** on PB4

### Initial OSCCAL Value

If EEPROM byte 0 is valid (not 0xFF), the calibration program will load it automatically:

```c
uint8_t stored = eeprom_read_byte((uint8_t*)EEPROM_OSCCAL_ADDR);
if (stored != 0xFF) {
    OSCCAL = stored;         // Use stored calibration
} else {
    OSCCAL = 0x5C;          // Default starting value
}
_delay_ms(1000);
```

**If EEPROM was erased (0xFF):**
- The program defaults to `OSCCAL = 0x5C`
- This should give approximately 8MHz (±25%)
- Adjust manually if needed

### Manual Adjustment

If the default value doesn't give you 7.8-8.2MHz (1.95-2.05MHz on CKOUT):

1. **Edit calibrate.c** and change the default value:
   ```c
   OSCCAL = 0x60;  // Try different values: 0x50, 0x60, 0x70, etc.
   ```

2. **Recompile and flash:**
   ```bash
   make clean
   make MCU=attiny45 BAUD=1200
   avrdude -c usbasp -p t45 -U flash:w:calibrate.hex
   ```

3. **Measure frequency** on PB4

4. **Repeat** until frequency is in range 7.8-8.2MHz

**Frequency adjustment guide:**
- Too low (< 7.8MHz) → increase OSCCAL
- Too high (> 8.2MHz) → decrease OSCCAL
- Each step changes frequency by ~50-100kHz

---

## Step 3: Fine Tuning with Calibrate Program

Once the frequency is in the correct range (7.8-8.2MHz), the UART should work and you can use the interactive calibration program.

### Setup Serial Connection

**Connections:**
- PB0 (pin 5) → TX → RX on USB-serial adapter
- PB2 (pin 7) → RX → TX on USB-serial adapter  
- GND → GND

**Serial settings:**
- **Baud rate: 1200**
- **Data bits: 8**
- **Parity: None**
- **Stop bits: 1**

### Running Calibration

1. **Flash the calibrate program:**
   ```bash
   avrdude -c usbasp -p t45 -U flash:w:calibrate_t45.hex
   ```

2. **Open serial terminal** at 1200 baud, 8N1

3. **You should see:**
   ```
   OSCCAL Calibration
   Current OSCCAL: 0x5C
   EEPROM OSCCAL: 0xFF
   
   Commands:
     + : increase OSCCAL
     - : decrease OSCCAL
     s : save to EEPROM
     r : show current
   
   > 
   ```

4. **Monitor frequency on oscilloscope** (keep probe on PB4)

5. **Adjust OSCCAL:**
   - Press **+** to increase frequency (each press adds 1 to OSCCAL)
   - Press **-** to decrease frequency (each press subtracts 1)
   - Target: Exactly **2.000MHz on PB4** (8.000MHz internal)

6. **Fine-tune until perfect:** Aim for the closest you can get to 2.000MHz

7. **Save to EEPROM:** Press **s** when satisfied
   ```
   > s
   Saved 0x5C to EEPROM
   ```

### Valid OSCCAL Range

**Example from testing:**
- **OSCCAL = 0x56:** Frequency too low - serial port stops working
- **OSCCAL = 0x57-0x64:** Serial port works, adjustable range
- **OSCCAL = 0x5C:** Typically gives 8.0MHz ±0.2%
- **OSCCAL = 0x65:** Frequency too high - serial port stops working

**Your chip's range may vary!** The valid range is typically 8-15 steps wide around the optimal value.

---

## Step 4: Final Fuse Configuration (Optional)

After calibration is complete, you can optionally disable CKOUT and adjust CKDIV8 for your application.

### To Disable Clock Output

**If you need PB4 for other purposes:**

```bash
# ATtiny13 - disable CKOUT (set bit 6 = 1)
avrdude -c usbasp -p t13 -U lfuse:w:0x6A:m

# ATtiny25/45/85 - disable CKOUT
avrdude -c usbasp -p t85 -U lfuse:w:0xE2:m
```

### To Enable Clock Divider (/8)

**For 1MHz operation instead of 8MHz:**

```bash
# ATtiny25/45/85 - 1MHz with CKOUT disabled
avrdude -c usbasp -p t85 -U lfuse:w:0x62:m
```

**⚠️ IMPORTANT:** 
- **DO NOT** change EESAVE back to 1 - this will erase your calibration!
- Keep EESAVE = 0 permanently (hfuse = 0xD7 or 0xF7)

---

## Using Calibrated Values in Your Programs

Once calibrated, your programs should load OSCCAL from EEPROM at startup:

```c
#include <avr/eeprom.h>

#define EEPROM_OSCCAL_ADDR 0

int main(void) {
    // Load calibrated OSCCAL value
    uint8_t cal = eeprom_read_byte((uint8_t*)EEPROM_OSCCAL_ADDR);
    if (cal != 0xFF) {
        OSCCAL = cal;
    }
    
    // Your code here...
}
```

---

## Troubleshooting

### No Serial Output / Garbage Characters

**Possible causes:**
1. **Wrong baud rate** - Make sure terminal is set to 1200 baud
2. **OSCCAL too far off** - Manually adjust in code and reflash
3. **Wrong connections** - Verify TX/RX are not swapped
4. **Fuses incorrect** - Verify with avrdude

### EEPROM Value Lost After Programming

**Cause:** EESAVE fuse is set to 1  
**Fix:** Set EESAVE = 0 in high fuse byte

```bash
avrdude -c usbasp -p t85 -U hfuse:w:0xD7:m
```

### Calibration Drifts Over Time

**Normal behavior:** Internal RC oscillators drift with temperature and voltage changes (~1-3%)

**Solutions:**
- Use external crystal for critical timing
- Recalibrate at operating temperature/voltage
- Accept ±2% drift for UART applications

---

## Summary

1. **Set fuses:** CKOUT=0, CKDIV8=1, EESAVE=0
2. **Coarse calibration:** Adjust OSCCAL in code until 7.8-8.2MHz
3. **Fine tuning:** Use calibrate program at 1200 baud with +/- commands
4. **Save:** Press 's' to store final value in EEPROM
5. **Keep EESAVE=0** permanently to preserve calibration

**Typical results:** ±0.2% frequency accuracy, sufficient for UART up to 9600 baud.

