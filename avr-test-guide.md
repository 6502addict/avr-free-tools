# Minimal AVR Test Setup Guide

## 🔌 Absolute Minimum Hardware Setup

### Power Connections (ALL chips):
```
Lab PSU +5V → VCC pin
Lab PSU GND → GND pin
```

### Bypass Capacitor (CRITICAL!):
```
100nF ceramic → Between VCC and GND, AS CLOSE TO CHIP AS POSSIBLE
```

## 📍 Pin Connections by Chip

### ATtiny13/25/45/85 (8-pin DIP):
```
        ┌─────┐
  RST ──┤1   8├── VCC (+5V)
  RX  ──┤2   7├── (not used)
  TX  ──┤3   6├── (not used)
  GND ──┤4   5├── (not used)
        └─────┘

Physical pins:
- Pin 1: RESET (10kΩ pullup to VCC optional but recommended)
- Pin 2: PB3 → External Clock IN (if using external clock)
- Pin 3: PB4 (or PB0 for software UART RX)
- Pin 4: GND
- Pin 5: PB0 (software UART RX if not on pin 3)
- Pin 6: PB1 (software UART TX)
- Pin 7: PB2
- Pin 8: VCC (+5V)
```

### ATmega328P (28-pin DIP):
```
         ┌─────┐
   RST ──┤1  28├── (not used)
   RXD ──┤2  27├── (not used)
   TXD ──┤3  26├── (not used)
         │...  │
   GND ──┤8  22├── GND
   CLK ──┤9  21├── AREF (connect to VCC)
         │...  │
   VCC ──┤7  20├── VCC (+5V)
         └─────┘

Key pins:
- Pin 1: RESET (10kΩ pullup to VCC)
- Pin 2: RXD (PD0) → FTDI TX
- Pin 3: TXD (PD1) → FTDI RX
- Pin 7,20: VCC (+5V)
- Pin 8,22: GND
- Pin 9: XTAL1 → External Clock IN (if using)
- Pin 21: AREF → Connect to VCC for ADC stability
```

### ATmega2560 (100-pin, using Arduino Mega pinout reference):
```
For minimal test, use Arduino Mega board connections:
- RX0: Pin 0 (PE0) → FTDI TX
- TX0: Pin 1 (PE1) → FTDI RX
- VCC: Multiple pins (connect at least 2)
- GND: Multiple pins (connect at least 2)
- External Clock: XTAL1 pin if needed
```

## 🔗 FTDI Serial Cable (3-wire) Connection

### ⚠️ CRITICAL: Check your FTDI voltage!
```bash
# FTDI 5V version:
FTDI TX  → AVR RX
FTDI RX  → AVR TX  
FTDI GND → AVR GND

# FTDI 3.3V version (NEEDS LEVEL SHIFTER!):
Use resistor divider on AVR TX → FTDI RX:
AVR TX ──[2.2kΩ]──┬──→ FTDI RX
                   │
                [3.3kΩ]
                   │
                  GND
```

## 🔶 External Clock Connection

### From Function Generator:
```
Function Generator Settings:
- Frequency: 8MHz (or desired)
- Waveform: SQUARE WAVE (important!)
- Amplitude: 5V peak-to-peak
- Offset: 2.5V (so signal swings 0V to 5V)
- Output impedance: 50Ω

Connection:
Generator OUT → 100Ω resistor → AVR CLKI/XTAL1 pin
Generator GND → AVR GND
```

### ⚠️ Signal Level Check:
```
CRITICAL: Measure with scope first!
- Signal MUST be 0V to 5V
- NOT -2.5V to +2.5V
- NOT 0V to 10V
```

## 🔧 Fuse Settings for External Clock

### Before connecting external clock, set fuses:

```bash
# ATtiny85 - External Clock
avrdude -p t85 -c usbtiny -U lfuse:w:0xE0:m

# ATtiny45 - External Clock  
avrdude -p t45 -c usbtiny -U lfuse:w:0xE0:m

# ATtiny13 - External Clock
avrdude -p t13 -c usbtiny -U lfuse:w:0x78:m

# ATmega328P - External Clock
avrdude -p m328p -c usbtiny -U lfuse:w:0xE0:m

# ATmega2560 - External Clock
avrdude -p m2560 -c usbtiny -U lfuse:w:0xE0:m
```

## 🧪 Test Sequence

### 1. Power Test (No clock, No program):
```
1. Connect only VCC, GND, and 100nF cap
2. Apply 5V
3. Measure current (should be < 10mA)
4. Check chip doesn't get hot
```

### 2. Internal Clock Test:
```
1. Program with internal clock fuses
2. Upload blink program
3. Verify LED blinks
```

### 3. External Clock Test:
```
1. Set external clock fuses
2. Connect function generator
3. Try to communicate with programmer
4. Upload program
```

### 4. Serial Test:
```
1. Connect FTDI cable
2. Upload serial echo program
3. Open terminal: screen /dev/ttyUSB0 9600
4. Type characters, see echo
```

### 5. Hex Display Test (ATmega only):
```
1. Upload mixed C/ASM hex display
2. Send data via terminal
3. Verify hex output format
```

## 📊 Minimal Parts List Per Chip

### For ANY test:
- [ ] 100nF ceramic capacitor (bypass)
- [ ] Breadboard or socket
- [ ] Jumper wires

### For better stability:
- [ ] 10kΩ resistor (RESET pullup)
- [ ] 10µF electrolytic (power filtering)

### For external clock:
- [ ] 100Ω resistor (signal coupling)
- [ ] Coax cable from generator

### For FTDI with 3.3V cable:
- [ ] 2.2kΩ resistor
- [ ] 3.3kΩ resistor

## 🐛 Troubleshooting

### "Cannot communicate with chip":
1. Check power (exactly 5V?)
2. Check GND connections
3. Try slower ISP clock: `-B 10`
4. If external clock set: Is clock running?

### "Garbage on serial":
1. Check baud rate matches
2. Check TX/RX not swapped
3. Measure actual clock frequency
4. Check FTDI cable voltage level

### "Chip gets hot":
1. IMMEDIATELY disconnect power
2. Check no pins shorted
3. Check VCC polarity correct
4. Check not exceeding 5.5V

### Recovery from wrong fuses:
```bash
# High-voltage programming mode (if available)
# OR
# Supply expected clock to XTAL1:
# - 1MHz from Arduino
# - 32.768kHz from RTC crystal
# - Even 50Hz/60Hz from transformer!
```

## 📝 Test Log Template

```
Date: ___________
Chip: ATtiny[13/25/45/85] ATmega[328P/2560/1284P]
Clock: Internal[8MHz/1MHz] External[_____MHz]
Voltage: _____V (measured)
Current: _____mA (measured)

Test Results:
[ ] Power OK
[ ] Programmer detects chip
[ ] Fuses read successfully
[ ] Program uploads
[ ] Blink LED works
[ ] Serial echo works
[ ] Hex display works

Notes:
_________________________________
_________________________________
```

---

**Pro Tips:**
1. Always connect bypass capacitor FIRST
2. Double-check FTDI voltage (3.3V vs 5V)
3. Start with internal clock before trying external
4. Keep wires SHORT (especially clock and serial)
5. Use scope to verify clock signal levels
6. Save working fuse settings for each chip

Good luck with testing! 🚀