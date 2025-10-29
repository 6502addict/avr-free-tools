#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // Configure PB4, PB5, PB6, PB7 as outputs
    DDRB |= (1 << PB4) | (1 << PB5) | (1 << PB6) | (1 << PB7);
    
    // Counters for each pin (different ratios: 1:2:4:8)
    uint8_t counter = 0;
    
    while (1) {
        counter++;
        
        // PB4 toggles every 8 cycles = 1x frequency
        if ((counter & 0x07) == 0) {
            PORTB ^= (1 << PB4);
        }
        
        // PB5 toggles every 4 cycles = 2x frequency
        if ((counter & 0x03) == 0) {
            PORTB ^= (1 << PB5);
        }
        
        // PB6 toggles every 2 cycles = 4x frequency
        if ((counter & 0x01) == 0) {
            PORTB ^= (1 << PB6);
        }
        
        // PB7 toggles every cycle = 8x frequency
        PORTB ^= (1 << PB7);
        
        _delay_us(50);  // Adjust this for desired base frequency
    }
    
    return 0;
}
