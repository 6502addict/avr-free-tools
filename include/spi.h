#ifndef SPI_H
#define SPI_H

// Function prototypes - same as your original API
void    spi_init(uint8_t divisor, uint8_t cpol, uint8_t cpha);
void    spi_set_divisor(uint8_t divisor);
void    spi_set_mode(uint8_t cpol, uint8_t cpha);
void    spi_cs_low(void);
void    spi_cs_high(void);
uint8_t spi_calculate_divisor(uint16_t frequency_khz);
void    spi_set_frequency_khz(uint16_t frequency);
uint8_t spi_transfer(uint8_t data);

#endif // SPI_H


