#include <avr/io.h>
#include <stdint.h>
#include "spi.h"

/* this library is very dependant of the spi configuration used
 * everything is simple if the spi device is connected directly to the avr spi controller
 * not that if an alternate pin is used for spi_cs the standard spi_cs of the avr
 * must also be set to activate the spi transmission
 *
 * if you want to use a non standard configuration you have to add it in these defines
 * these configuration as for the most common arduino sdcard shield for the market
 * more precisely the adafruit data logger (old) or REVB and the seedstudio sdcard shield v4.x
 * not that all shield will work with an arduino uno
 * but the old adafruit datalogger will not work on arduino mega
 *
 * also not that many chinese sdcard shields are bad copy of the original adafruit datalogger
 * and won't work on arduino mega 2560
 */ 

#define SEEDSTUDIO_SDCARD_SHIELD

// Define SPI pins based on MCU type
#if defined(__AVR_ATmega328P__)
// ATmega328P (Arduino Uno) - Hardware SPI
// these pins are defining the standard avr spi port pin
#define SPI_DDR     DDRB
#define SPI_PORT    PORTB
#define HW_SS_PIN   PB2   // Hardware SS (must be output for master mode)
#define MOSI_PIN    PB3  
#define MISO_PIN    PB4  
#define SCK_PIN     PB5  
// CS for SD card shields - typically on Digital 4 (Port D)
#if defined(SEEDSTUDIO_SDCARD_SHIELD)
#define CS_DDR      DDRD
#define CS_PORT     PORTD
#define CS_PIN      PD4   // D4
#endif 

// CS for SD card shields (same as HW_SS on Uno)
// there is no arduino board using this chip so
// we just consider that the spi device is just attached to the avr hardware spi port
#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__)
// ATmega1284/1284P - Hardware SPI
#define SPI_DDR     DDRB
#define SPI_PORT    PORTB
#define HW_SS_PIN   PB4   // Hardware SS
#define MOSI_PIN    PB5
#define MISO_PIN    PB6
#define SCK_PIN     PB7

#elif defined(__AVR_ATmega2560__)
// ATmega2560 (Arduino Mega) - Hardware SPI on ICSP header
#define SPI_DDR     DDRB
#define SPI_PORT    PORTB
#define HW_SS_PIN   PB0   // D53 - Hardware SS (MUST be output for master mode!)
#define MOSI_PIN    PB2   // D51 (ICSP-4)
#define MISO_PIN    PB3   // D50 (ICSP-1)
#define SCK_PIN     PB1   // D52 (ICSP-3)
#if defined(SEEDSTUDIO_SDCARD_SHIELD)
#define CS_DDR      DDRG
#define CS_PORT     PORTG
#define CS_PIN      PG5   // D4
#endif 


#else
    #error "Unsupported MCU"
#endif

void spi_cs_low(void) {
  SPI_PORT &= ~(1 << HW_SS_PIN);
#ifdef CS_PORT
  CS_PORT  &= ~(1 << CS_PIN);
#endif
}

void spi_cs_high(void) {
  SPI_PORT |= (1 << HW_SS_PIN);
#ifdef CS_PORT
  CS_PORT  |= (1 << CS_PIN);
#endif
}

void spi_set_divisor(uint8_t divisor) {
  uint8_t spr_bits = 0;
  
  // Map divisor to AVR SPI clock select bits
  if (divisor >= 128) spr_bits = 0x03;      // /128
  else if (divisor >= 64) spr_bits = 0x02;  // /64
  else if (divisor >= 16) spr_bits = 0x01;  // /16
  else spr_bits = 0x00;                     // /4
  
  SPCR = (SPCR & 0xFC) | (spr_bits & 0x03);
}

void spi_set_mode(uint8_t xcpol, uint8_t xcpha) {
  SPCR &= ~((1 << CPOL) | (1 << CPHA));
  if (xcpol) 
    SPCR |= (1 << CPOL);
  if (xcpha) 
    SPCR |= (1 << CPHA);
}

uint8_t spi_calculate_divisor(uint16_t frequency_khz) {
  const uint32_t cpu_khz = F_CPU / 1000;
  uint16_t divisor = cpu_khz / frequency_khz;
  
  if (divisor > 255) divisor = 255;
  if (divisor < 4) divisor = 4;
  
  return (uint8_t)divisor;
}

void spi_set_frequency_khz(uint16_t frequency) {
  uint8_t divisor = spi_calculate_divisor(frequency);
  spi_set_divisor(divisor);
}

void spi_init(uint8_t divisor, uint8_t cpol, uint8_t cpha) {
#if defined(__AVR_ATmega2560__)
  // Enable SPI power on ATmega2560
  PRR0 &= ~(1 << PRSPI);
#endif
  
#ifdef CS_DDR
  // Configure CS pin (may be on different port than SPI)
  CS_DDR |= (1 << CS_PIN);
#endif 
  
  // CRITICAL: Hardware SS pin MUST be output for master mode to work!
  // Even if we use a different pin for CS, SS must be configured as output
  SPI_DDR |= (1 << HW_SS_PIN) | (1 << MOSI_PIN) | (1 << SCK_PIN);
  SPI_PORT |= (1 << HW_SS_PIN);  // Keep SS high
  
  // Set MISO as input
  SPI_DDR &= ~(1 << MISO_PIN);
  
  // Enable SPI as Master
  SPCR &= ~((1 << DORD) | (1 << CPOL) | (1 << CPHA));
  SPCR = (1 << SPE) | (1 << MSTR);
  
  spi_set_mode(cpol, cpha);
  spi_set_divisor(divisor);
  spi_cs_high();
}

uint8_t spi_transfer(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1 << SPIF)));
  return SPDR;
}
