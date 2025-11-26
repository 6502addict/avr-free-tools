# ATtiny External Clock Configuration

## 🔌 Two External Clock Options

### Option 1: External Crystal Oscillator (XTAL)
- Connect crystal between XTAL1 (PB3) and XTAL2 (PB4)
- Add 18-22pF capacitors to ground
- Uses 2 I/O pins
- More stable, less noise

### Option 2: External Clock Signal (EXTCLK)
- Feed external clock directly to CLKI pin (PB3)
- From signal generator, another MCU, or oscillator module
- Uses only 1 I/O pin
- **This is what you want for an 8MHz generator!**

## 📍 Pin Connections by Chip

| Chip | XTAL1/CLKI Pin | XTAL2 Pin | Physical Pin# (DIP-8) |
|------|---------------|-----------|----------------------|
| ATtiny13 | PB3 | PB4 | Pin 2 & 3 |
| ATtiny25/45/85 | PB3 | PB4 | Pin 2 & 3 |

## ⚡ Using External Clock Signal (8MHz Generator)

### Hardware Connection:
```
   8MHz Generator
        │
        ├─────→ PB3 (Pin 2) - CLKI input
        │
   ATtiny85
```

**Important:** 
- Input signal: 0-VCC square wave
- Max frequency: 20MHz (at 5V)
- Signal must be clean (low jitter)

## 🔧 Fuse Settings

### ATtiny85 - External Clock Input
```bash
# External Clock on CLKI pin (PB3)
# CKSEL[3:0] = 0000
avrdude -p t85 -c usbtiny -U lfuse:w:0xE0:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m
```

### ATtiny45 - External Clock Input
```bash
avrdude -p t45 -c usbtiny -U lfuse:w:0xE0:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m
```

### ATtiny25 - External Clock Input
```bash
avrdude -p t25 -c usbtiny -U lfuse:w:0xE0:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m
```

### ATtiny13 - External Clock Input
```bash
# External Clock on CLKI pin
# CKSEL[1:0] = 00
avrdude -p t13 -c usbtiny -U lfuse:w:0x78:m -U hfuse:w:0xFF:m
```

## 📊 Fuse Bit Breakdown (ATtiny25/45/85)

### Low Fuse Byte for Clock Selection:
```
CKSEL[3:0] Clock Source
---------- ------------
1111       Crystal 8-20MHz
1110       Crystal 8-20MHz
1101       Crystal 6-8MHz  
1100       Crystal 6-8MHz
1011       Crystal 4-8MHz
1010       Crystal 4-8MHz
1001       Crystal 3-4MHz
1000       Crystal 3-4MHz
0111       Crystal 0.9-3MHz
0110       Crystal 0.9-3MHz
0101       Crystal 0.4-0.9MHz
0100       Crystal 0.4-0.9MHz
0011       Internal 128kHz
0010       Internal 8MHz
0001       Reserved
0000       External Clock ← YOUR SETTING!
```

## ⚠️ Warning: Pin Loss

When using external clock:
- **XTAL mode**: Loses PB3 and PB4 (2 pins)
- **EXTCLK mode**: Loses only PB3 (1 pin)

On ATtiny85, this reduces usable I/O from 6 to 5 pins.

## 💻 Updated Makefile Target

```makefile
# Add to your Makefile
fuses-extclk:
	@echo "Setting fuses for External Clock input..."
ifeq ($(MCU),ATtiny13)
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U lfuse:w:0x78:m \
		-U hfuse:w:0xFF:m
endif
ifeq ($(MCU),ATtiny25)
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U lfuse:w:0xE0:m \
		-U hfuse:w:0xDF:m \
		-U efuse:w:0xFF:m
endif
ifeq ($(MCU),ATtiny45)
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U lfuse:w:0xE0:m \
		-U hfuse:w:0xDF:m \
		-U efuse:w:0xFF:m
endif
ifeq ($(MCU),ATtiny85)
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-U lfuse:w:0xE0:m \
		-U hfuse:w:0xDF:m \
		-U efuse:w:0xFF:m
endif
```

## 🔄 Recovery from Wrong Fuse Settings

If you set wrong fuses and can't communicate with the chip:

### Method 1: High-Voltage Programming
- Requires 12V programmer
- Can always recover any fuse setting

### Method 2: External Clock Injection
- Feed external clock to CLKI/XTAL1
- Try different frequencies (1kHz to 20MHz)
- Once communication restored, reprogram fuses

### Method 3: Clock Generation with Arduino
```c
// Generate 1MHz clock on pin 9 (Arduino Uno)
void setup() {
  // Timer1, 1MHz on OC1A (pin 9)
  pinMode(9, OUTPUT);
  TCCR1A = 0x40;  // Toggle OC1A
  TCCR1B = 0x09;  // No prescaler, CTC mode
  OCR1A = 7;      // 16MHz / (2*(7+1)) = 1MHz
}
void loop() {}
```

Connect Arduino pin 9 to ATtiny CLKI pin to recover.

## 📝 Code Considerations

When using external clock in your assembly code:
1. Update F_CPU definition in Makefile
2. Recalculate timer values for delays/UART
3. No changes needed to instruction timing

```makefile
# Example for 8MHz external clock
F_CPU = 8000000
```

## 🎯 Use Cases for External Clock

1. **Synchronization**: Multiple MCUs with same clock
2. **Precise Timing**: GPS disciplined oscillator
3. **Variable Frequency**: Testing at different speeds
4. **Special Frequencies**: Non-standard baud rates
5. **Clock Recovery**: Fix wrong fuse settings

---

**Note:** Always double-check fuse values before programming! Wrong clock fuses can "brick" your chip temporarily until you provide the expected clock source.