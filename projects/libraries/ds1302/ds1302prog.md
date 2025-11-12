# DS1302 Programming Guide

## Data Format
All time/date values are stored in **BCD (Binary Coded Decimal)**:
- Example: 25 seconds = 0x25 (not 0x19)
- Conversion: `BCD = (tens << 4) | units`

## Clock Registers (Read/Write addresses)

| Register | Read | Write | Format | Range |
|----------|------|-------|--------|-------|
| Seconds  | 0x81 | 0x80 | 0bCH-tens-units | 00-59 |
| Minutes  | 0x83 | 0x82 | 0b0-tens-units | 00-59 |
| Hours    | 0x85 | 0x84 | See below | 00-23 or 1-12 |
| Date     | 0x87 | 0x86 | 0b00-tens-units | 01-31 |
| Month    | 0x89 | 0x88 | 0b000-tens-units | 01-12 |
| Day      | 0x8B | 0x8A | 0b00000-day | 1-7 |
| Year     | 0x8D | 0x8C | 0b-tens-units | 00-99 |
| Control  | 0x8F | 0x8E | 0bWP-000000 | - |

## Hours Register Format
**24-hour mode (bit 7 = 0):**
- Bits 0-3: units
- Bits 4-5: tens (0-2)
- Bit 7: 0

**12-hour mode (bit 7 = 1):**
- Bits 0-3: units
- Bit 4: tens (0-1)
- Bit 5: AM/PM (0=AM, 1=PM)
- Bit 7: 1

## Critical Bits

**Seconds Register - Bit 7 (CH - Clock Halt):**
- `CH = 1` → Clock stopped (standby, <100nA)
- `CH = 0` → Clock running
- **Must clear CH to start clock after power-up**

**Control Register - Bit 7 (WP - Write Protect):**
- `WP = 1` → All registers read-only
- `WP = 0` → Registers writable
- **Must clear WP before any write operation**

## Programming Sequence

**To set time/date:**
```
1. Write 0x00 to Control register (0x8E) → disable WP
2. Write seconds with CH=0 to start clock
3. Write other time/date registers in BCD
4. (Optional) Write 0x80 to Control register → enable WP
```

**To read time/date:**
```
1. Read seconds register, mask bit 7: value & 0x7F
2. Read other registers
3. Convert all values from BCD to decimal
```

## RAM
- **31 bytes** available
- Addresses: 0xC0-0xFC (write), 0xC1-0xFD (read)
- No BCD format - direct binary values (0x00-0xFF)

## Burst Mode
**Clock burst:** Read/write all 8 clock registers at once
- Write: 0xBE
- Read: 0xBF

**RAM burst:** Read/write all 31 RAM bytes at once
- Write: 0xFE
- Read: 0xFF

## Important Notes
1. **Always clear WP (write 0x00 to 0x8E) before writing**
2. **Power-up state is undefined** - must initialize CH and WP
3. **Writing seconds resets countdown** - write all registers within 1 second
4. Day-of-week (1-7) is user-defined, must be sequential
5. Illogical dates cause undefined behavior