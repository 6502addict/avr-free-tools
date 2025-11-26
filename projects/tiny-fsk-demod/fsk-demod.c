#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>


#define INPUT_PIN PB3
#define OUTPUT_PIN PB0
#define STATUS_LED PB1
#define CLOCK_PIN PB2


// Threshold to distinguish 1200Hz from 2400Hz
// With prescaler 64: midpoint between the two frequencies

#define THRESHOLD_LOW  ((F_CPU / 64UL) / 1500UL)  // ~1500 Hz
#define THRESHOLD_HIGH ((F_CPU / 64UL) / 2100UL)  // ~2100 Hz

volatile uint8_t last_time = 0;
volatile uint8_t transition_count = 0;

ISR(PCINT0_vect) {
    if (PINB & (1 << INPUT_PIN)) {
        uint8_t current_time = TCNT0;
        uint8_t period = current_time - last_time;
        last_time = current_time;
        
        if (period > THRESHOLD_LOW) {
            PORTB &= ~(1 << OUTPUT_PIN);  // 1200Hz = '0'
            PORTB |= (1 << STATUS_LED);   // LED ON = locked
        } else if (period < THRESHOLD_HIGH) {
            PORTB |= (1 << OUTPUT_PIN);   // 2400Hz = '1'
            PORTB |= (1 << STATUS_LED);   // LED ON = locked
        } else {
            PORTB &= ~(1 << STATUS_LED);  // LED OFF = unsync
        }
	
	// Generate 300Hz clock: toggle every 4 transitions (1200Hz/4 = 300Hz)
        if (++transition_count >= 4) {
            PORTB ^= (1 << CLOCK_PIN);
            transition_count = 0;
        }
    }
}

int main(void) {
#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    OSCCAL = eeprom_read_byte((uint8_t*)0);
#endif

    // Setup input pin (no pullup for FSK signal)
    DDRB &= ~(1 << INPUT_PIN);
    PORTB &= ~(1 << INPUT_PIN);
    
    // Setup output pin
    DDRB |= (1 << OUTPUT_PIN);
    PORTB &= ~(1 << OUTPUT_PIN);
    
    // Timer0 free-running with prescaler 64
    TCCR0A = 0;
    TCCR0B = (1 << CS01) | (1 << CS00);
    TCNT0 = 0;
    
    // Enable pin change interrupt on PB3
    PCMSK = (1 << INPUT_PIN);
    GIMSK = (1 << PCIE);
    
    sei();
    
    while (1) {
        sleep_mode();
    }
}
