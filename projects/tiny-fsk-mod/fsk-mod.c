#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#define USE_CLOCK 0  // Set to 1 for clock-driven, 0 for async

#define FREQ_OUT_PIN PB0  // FSK output
#define DATA_PIN PB3      // Serial data input
#if USE_CLOCK
#define CLOCK_PIN PB4     // Serial clock input (if USE_CLOCK=1)
#endif

#define FREQ_MARK  2400   // Logic '1'
#define FREQ_SPACE 1200   // Logic '0'
#define PRESCALER 64

#define OCR_MARK  ((F_CPU / (2UL * PRESCALER * FREQ_MARK)) - 1)
#define OCR_SPACE ((F_CPU / (2UL * PRESCALER * FREQ_SPACE)) - 1)

ISR(PCINT0_vect) {
#if USE_CLOCK
    // Clock-driven: sample data on clock rising edge
    if (PINB & (1 << CLOCK_PIN)) {
        uint8_t state = (PINB & (1 << DATA_PIN)) ? 1 : 0;
        OCR0A = state ? OCR_MARK : OCR_SPACE;
    }
#else
    // Async: change frequency on data pin change
    uint8_t state = (PINB & (1 << DATA_PIN)) ? 1 : 0;
    OCR0A = state ? OCR_MARK : OCR_SPACE;
#endif
}

int main(void) {
#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    OSCCAL = eeprom_read_byte((uint8_t*)0);
#endif

    // Setup output
    DDRB |= (1 << FREQ_OUT_PIN);
    
    // Setup data input with pullup
    DDRB &= ~(1 << DATA_PIN);
    PORTB |= (1 << DATA_PIN);
    
#if USE_CLOCK
    // Setup clock input with pullup
    DDRB &= ~(1 << CLOCK_PIN);
    PORTB |= (1 << CLOCK_PIN);
    PCMSK = (1 << CLOCK_PIN);  // Interrupt on clock
#else
    PCMSK = (1 << DATA_PIN);   // Interrupt on data
#endif
    
    // Timer0 CTC mode, toggle OC0A, prescaler 64
    TCCR0A = (1 << COM0A0) | (1 << WGM01);
    TCCR0B = (1 << CS01) | (1 << CS00);
    OCR0A = OCR_MARK;
    
    GIMSK = (1 << PCIE);
    sei();
    
    while (1) {
        sleep_mode();
    }
}
