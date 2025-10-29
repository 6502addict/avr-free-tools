#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <spi.h>

int main(void) {
    // Test slow speed (100 kHz for SD init)
    spi_init(128, 0, 0);  // /128 = 125 kHz (closest to 100 kHz)
    spi_cs_low();
    
    spi_set_frequency_khz(100);
    spi_cs_low();
    
    // Send pattern continuously at high speed
    while (1) {
        spi_transfer(0xAA);
    }
    
    return 0;
}
