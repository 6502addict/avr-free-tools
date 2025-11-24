# AVR 3.3V Operation Guide 

## ⚡ Voltage vs Frequency Limits

### Critical Rule: Lower voltage = Lower max frequency!

| Chip | Voltage Range | Max Frequency |
|------|--------------|---------------|
| **ATtiny13** | 2.7-5.5V | 10MHz @ 2.7V, 20MHz @ 5V |
| **ATtiny25/45/85** | 2.7-5.5V | 10MHz @ 2.7V, 20MHz @ 5V |
| **ATmega328P** | 1.8-5.5V | 4MHz @ 1.8V, 10MHz @ 2.7V, 20MHz @ 5V |
| **ATmega2560** | 1.8-5.5V | 4MHz @ 1.8V, 10MHz @ 2.7V, 20MHz @ 5V |
| **ATmega1284P** | 1.8-5.5V | 4MHz @ 1.8V, 10MHz @ 2.7V, 20MHz @ 5V |

### Safe Operating Frequencies at 3.3V:
- **ATtiny series**: Max 10-12MHz 
- **ATmega series**: Max 12-13MHz
- **Recommended**: 8MHz for stability margin

## 🔌 Power Supply Setup for 3.3V

### Option 1: Lab Power Supply
```
Lab PSU → Set to 3.3V
        → Current limit ~100mA
        → Connect to AVR VCC + GND
        → Add 100nF + 10µF caps
```

### Option 2: LDO Regulator (from 5V)
```
5V → [AMS1117-3.3 or MCP1700-3.3] → 3.3V
     ├── 10µF                       ├── 10µF
     └── GND                        ├── 100nF
                                    └── To AVR
```

### Option 3: Direct from 3.3V FTDI
```
FTDI 3.3V pin → AVR VCC (max ~50mA!)
FTDI GND → AVR GND
⚠️ Limited current - OK for basic tests only
```

## 📡 FTDI Connection at 3.3V

### With 3.3V FTDI Cable (BEST):
```
Simple direct connection:
FTDI TX (3.3V) → AVR RX
FTDI RX (3.3V) → AVR TX  
FTDI GND → AVR GND
FTDI VCC (3.3V) → AVR VCC (optional power)
```

### With 5V FTDI Cable (NEEDS LEVEL SHIFTING):
```
Option 1: Resistor Divider (TX only)
5V FTDI TX ──[10kΩ]──┬──→ AVR RX (3.3V)
                      │
                   [20kΩ]
                      │
                     GND

AVR TX (3.3V) ────────→ 5V FTDI RX (usually OK)

Option 2: Proper Level Shifter
Use 74LVC245 or TXB0104 bidirectional shifter
```

## 🕐 Clock Configuration for 3.3V

### Internal Oscillator Settings:
```bash
# ATtiny85 - 8MHz internal (safe at 3.3V)
avrdude -p t85 -c usbtiny -U lfuse:w:0xE2:m

# ATtiny45 - 8MHz internal
avrdude -p t45 -c usbtiny -U lfuse:w:0xE2:m

# ATtiny13 - 9.6MHz internal / 8 = 1.2MHz (very safe)
avrdude -p t13 -c usbtiny -U lfuse:w:0x6A:m

# ATmega328P - 8MHz internal
avrdude -p m328p -c usbtiny -U lfuse:w:0xE2:m

# ATmega2560 - 8MHz internal
avrdude -p m2560 -c usbtiny -U lfuse:w:0xE2:m
```

### External Clock/Crystal at 3.3V:
```
Maximum safe frequencies:
- 8MHz crystal: PERFECT for 3.3V
- 12MHz crystal: OK but at limit
- 16MHz crystal: TOO FAST - will be unstable!

Function Generator Settings for 3.3V:
- Frequency: 8MHz max
- Amplitude: 3.3V peak-to-peak
- Offset: 1.65V (signal swings 0V to 3.3V)
```

## 🔧 Fuse Calculator for 3.3V Operation

### ATtiny85 Optimal 3.3V Settings:
```bash
# 8MHz internal, BOD at 2.7V
Low:  0xE2  # 8MHz internal
High: 0xDC  # BOD enabled at 2.7V
Ext:  0xFF  # Self-programming enabled

avrdude -p t85 -c usbtiny \
  -U lfuse:w:0xE2:m \
  -U hfuse:w:0xDC:m \
  -U efuse:w:0xFF:m
```

### ATmega328P Optimal 3.3V Settings:
```bash
# 8MHz internal, BOD at 2.7V
Low:  0xE2  # 8MHz internal
High: 0xD5  # BOD at 2.7V
Ext:  0xFF  # BOD at 2.7V

avrdude -p m328p -c usbtiny \
  -U lfuse:w:0xE2:m \
  -U hfuse:w:0xD5:m \
  -U efuse:w:0xFF:m
```

## 📝 Code Modifications for 3.3V

### Makefile Changes:
```makefile
# For 3.3V operation at 8MHz
ifeq ($(MCU),atmega328p)
    F_CPU = 8000000UL  # Changed from 16MHz
    BAUD = 9600        # Lower baud for reliability
endif
```

### Serial Baud Rate Adjustment:
```c
// At 3.3V, use conservative baud rates:
// 8MHz clock: 9600, 19200, 38400 work well
// Avoid: 115200 (too fast for 8MHz)
```

### Timing Considerations:
```assembly
; For ATtiny software UART at 8MHz/3.3V
; 9600 baud is most reliable
.equ BAUD_9600_TIMER = 104  ; Same as before
.equ PRESCALER_BITS = (1<<CS01)  ; /8 prescaler
```

## ⚠️ Important 3.3V Considerations

### Brown-Out Detection (BOD):
```
Enable BOD at 2.7V to prevent corruption:
- Prevents flash corruption during power dips
- Ensures clean reset if voltage drops
- Critical for battery-powered projects
```

### Current Consumption:
```
At 3.3V vs 5V:
- Generally LOWER current consumption
- Better for battery operation
- Longer battery life
```

### Pin Drive Strength:
```
At 3.3V, I/O pins provide:
- Less current (max ~10mA vs ~20mA at 5V)
- Slower rise/fall times
- May need stronger pull-ups (4.7kΩ vs 10kΩ)
```

## 🧪 Test Sequence for 3.3V

1. **Verify Supply Voltage**
   ```
   Measure: Should be 3.25-3.35V
   Ripple: <50mV
   ```

2. **Start with Internal Oscillator**
   ```bash
   # Program fuses for 8MHz internal
   # Upload blink test
   # Verify timing is correct
   ```

3. **Test Serial Communication**
   ```bash
   # Use 9600 baud initially
   # Check with: screen /dev/ttyUSB0 9600
   ```

4. **Power Consumption Check**
   ```
   Typical at 3.3V, 8MHz:
   - ATtiny85: ~3-5mA active
   - ATmega328P: ~4-8mA active
   ```

## 🔄 Switching Between 3.3V and 5V

### Quick Reference Table:
| Parameter | 5V Operation | 3.3V Operation |
|-----------|-------------|----------------|
| Max Clock | 16-20MHz | 8-12MHz |
| UART Baud | 115200 OK | 9600-38400 better |
| BOD Setting | 4.3V | 2.7V |
| Pull-up R | 10kΩ | 4.7kΩ |
| Bypass Cap | 100nF | 100nF + 10µF recommended |

## 💡 Pro Tips for 3.3V

1. **Always start at 8MHz or less** - Never assume 16MHz will work
2. **Use BOD** - Prevents mysterious crashes and corruption
3. **Check FTDI voltage** - Mixing 3.3V/5V can damage chips
4. **Add more capacitance** - 3.3V systems benefit from extra filtering
5. **Lower baud rates** - More reliable at lower clock speeds
6. **Test thoroughly** - Edge cases appear more at 3.3V

---

**Remember**: At 3.3V, you're trading maximum performance for lower power consumption. Perfect for battery-powered projects!