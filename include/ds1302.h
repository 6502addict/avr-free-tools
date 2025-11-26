#ifndef DS1302_H
#define DS1302_H

// Define DS1302 pins based on MCU type (avoiding SPI conflicts)
#if defined(__AVR_ATmega328P__)
// ATmega328P (Arduino Uno) - Use Port D pins 5,6,7
#define DS1302_DDR      DDRC
#define DS1302_PORT     PORTC
#define DS1302_PIN      PINC
#define DS1302_RST      PC0   // A0
#define DS1302_SCLK     PC1   // A1
#define DS1302_IO       PC2   // A2

#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__)
// ATmega1284/1284P - Use Port B pins (free from SPI)
#define DS1302_DDR      DDRB
#define DS1302_PORT     PORTB
#define DS1302_PIN      PINB
#define DS1302_RST      PB0
#define DS1302_SCLK     PB1
#define DS1302_IO       PB2

#elif defined(__AVR_ATmega2560__)
// ATmega2560 (Arduino Mega) - Use Port G to avoid SPI conflict
#define DS1302_DDR      DDRF
#define DS1302_PORT     PORTF
#define DS1302_PIN      PINF
#define DS1302_RST      PF0   // A0
#define DS1302_SCLK     PF1   // A1
#define DS1302_IO       PF2   // A2

#elif defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
// ATtiny2313 - Use Port D (limited pins available)
#define DS1302_DDR      DDRD
#define DS1302_PORT     PORTD
#define DS1302_PIN      PIND
#define DS1302_RST      PD3
#define DS1302_SCLK     PD4
#define DS1302_IO       PD5

#elif defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
// ATtiny13/25/45/85 - Use Port B (UART on PB0/PB2, DS1302 on PB3/PB4/PB5 or PB1/PB3/PB4)
#define DS1302_DDR      DDRB
#define DS1302_PORT     PORTB
#define DS1302_PIN      PINB
#define DS1302_RST      PB1   // or PB5 if PB1 has LED
#define DS1302_SCLK     PB3
#define DS1302_IO       PB4

#else
#error "Unsupported MCU type for DS1302 library"
#endif

// DS1302 register addresses
#define DS1302_SEC      0x00
#define DS1302_MIN      0x01
#define DS1302_HOUR     0x02
#define DS1302_DATE     0x03
#define DS1302_MONTH    0x04
#define DS1302_DAY      0x05
#define DS1302_YEAR     0x06
#define DS1302_WP       0x07    // Write protect
#define DS1302_TRICKLE  0x08

#define DS1302_READ_BIT 0x01
#define DS1302_RAM_BIT  0x40
#define DS1302_CMD_BIT  0x80

// Function prototypes
void    ds1302_init(void);
void    ds1302_write_ram(uint8_t addr, uint8_t data);
uint8_t ds1302_read_ram(uint8_t addr);
void    ds1302_write_rtc(uint8_t addr, uint8_t data);
uint8_t ds1302_read_rtc(uint8_t addr);

#endif
