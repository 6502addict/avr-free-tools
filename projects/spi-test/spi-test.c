#include <avr/io.h>
#include <util/delay.h> 

#define SPI_CS    PG5
#define SPI_MOSI  PB2
#define SPI_MISO  PB3
#define SPI_SCK   PB1

void spi_init() {
  PRR0 &= ~(1 << PRSPI);  
  DDRG |= (1 << SPI_CS);
  PORTG |=  (1 << SPI_CS); 
  DDRB &= ~(1 << SPI_MISO); 
  DDRB |= (1 << SPI_SCK) | (1 << SPI_MOSI) | (1 << PB0);
  PORTB |= (1 << PB0);
  PORTG |= (1 << SPI_CS);
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
  SPSR &= ~(1 << SPI2X);
}

uint8_t spi_transfer(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1 << SPIF)))
    ;
  return SPDR;
}

int main() {
  spi_init();
  uint8_t command = 0xAA;
  uint8_t response;
  
  for (;;) {
    PORTG &= ~(1 << SPI_CS); 
    response = spi_transfer(command);
    PORTG |=  (1 << SPI_CS); 
  }

  (void) response;
  return 0;
}
