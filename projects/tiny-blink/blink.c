#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>

// Pin definitions
#define CLK_PIN     PB1
#define SEL_PIN     PB3

int main(void) {
#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)  
  OSCCAL = eeprom_read_byte((uint8_t*)0);
#endif

  // Configure SEL_PIN as input with pullup
  DDRB &= ~(1 << SEL_PIN);
  PORTB |= (1 << SEL_PIN);
  
  // Configure CLK_PIN as output
  DDRB |= (1 << CLK_PIN);
  
  // Setup Timer0 in CTC mode
  TCCR0A = (1 << WGM01);
  
  // Set prescaler to 1024
  TCCR0B = (1 << CS02) | (1 << CS00);
  
  // Set compare value based on SEL_PIN state
  // 60Hz if SEL_PIN is low, 50Hz if high
  if (PINB & (1 << SEL_PIN)) {
    OCR0A = 92;  // 50Hz
  } else {
    OCR0A = 76;  // 60Hz
  }
  
  // Enable Timer0 Compare Match A interrupt
#if defined(__AVR_ATtiny13__)
  TIMSK0 = (1 << OCIE0A);
#else
  TIMSK = (1 << OCIE0A);
#endif
  
  // Set sleep mode to idle
  set_sleep_mode(SLEEP_MODE_IDLE);
  
  // Enable global interrupts
  sei();
  
  // Main loop - sleep and wake on interrupt
  while (1) {
    sleep_mode();
  }
}

// Timer0 Compare Match A interrupt
#if defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
ISR(TIMER0_COMPA_vect) {
#else
ISR(TIM0_COMPA_vect) {
#endif
  static uint8_t divisor = 0;
  
  divisor++;
  if (divisor == 50) {
    PINB = (1 << CLK_PIN);  // Toggle CLK_PIN by writing 1 to PINB
    divisor = 0;
  }
}
