#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdbool.h>

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

// Configuration
#define FREQUENCY 1000      
#define BANDWIDTH_PERCENT 10     
#define MIN_VALID_FREQ 100       

// Input pin: INT0 (PB2 on ATtiny13, PD2 on ATtiny25/45/85/2313)
// Output pin: PB0 (available on all)
#define OUTPUT_DDR  DDRB
#define OUTPUT_PORT PORTB
#define OUTPUT_PIN  PB1

/*
ATtiny13: PB2 = physical pin 7
ATtiny25/45/85: PB2 = physical pin 7
ATtiny2313: PD2 = physical pin 6
*/

// Frequency measurement variables
volatile uint16_t period_ticks = 0;
volatile bool new_period = false;
volatile uint8_t timer_overflows = 0;
volatile uint16_t last_timer = 0;

// Timer overflow ISR - count overflows for long periods
#if defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny13A__)
ISR(TIM0_OVF_vect) {
#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
ISR(TIMER1_OVF_vect) {
#elif defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
ISR(TIMER1_OVF_vect) {
#endif
  timer_overflows++;
  if (timer_overflows > 250) {  // Timeout - no signal
    period_ticks = 0;
    timer_overflows = 0;
  }
 }

// INT0 ISR - triggered on rising edge
ISR(INT0_vect) {
  uint16_t current_timer;
  
#if defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny13A__)
  current_timer = TCNT0 | (timer_overflows << 8);
#else
  current_timer = TCNT1 | (timer_overflows << 8);
#endif
  
  period_ticks = current_timer - last_timer;
  last_timer = current_timer;
  timer_overflows = 0;
  new_period = true;
}

void init_tone_detector(void) {
  // Setup output pin
  OUTPUT_DDR |= (1 << OUTPUT_PIN);
  OUTPUT_PORT &= ~(1 << OUTPUT_PIN);
  
  // Setup INT0 input
#if defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
  DDRD &= ~(1 << PD2);      // INT0 is on PD2 for 2313
  PORTD |= (1 << PD2);      // Enable pullup
#else
  DDRB &= ~(1 << PB2);      // INT0 is on PB2 for all 8-pin tinys
  PORTB |= (1 << PB2);      // Enable pullup
#endif
  
  // Configure INT0 for rising edge
  MCUCR |= (1 << ISC01) | (1 << ISC00);
  
  // Enable INT0
  GIMSK |= (1 << INT0);
  
  // Setup timer for period measurement
#if defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny13A__)
  // ATtiny13: Use Timer0, prescaler 8
  TCCR0B = (1 << CS01);
  TIMSK0 = (1 << TOIE0);
#elif defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
  // ATtiny2313: Use Timer1, prescaler 8
  TCCR1B = (1 << CS11);
  TIMSK = (1 << TOIE1);
#else
  // ATtiny25/45/85: Use Timer1, prescaler 8
  TCCR1 = (1 << CS12);
  TIMSK = (1 << TOIE1);
#endif
}
 
// Calculate frequency from period ticks
uint16_t calculate_frequency(uint16_t ticks) {
  if (ticks == 0) return 0;
  
  // Timer runs at F_CPU/8
  uint32_t freq = (F_CPU / 8UL) / ticks;
  return (uint16_t)freq;
}

// Check if frequency is within bandwidth
bool freq_in_range(uint16_t measured, uint16_t target, uint8_t bandwidth_pct) {
  uint16_t bandwidth = (target * bandwidth_pct) / 100;
  uint16_t min_freq = target - (bandwidth / 2);
  uint16_t max_freq = target + (bandwidth / 2);
  return (measured >= min_freq && measured <= max_freq);
}

int main(void) {
  uint16_t current_freq = 0;
  
#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)  
  OSCCAL = eeprom_read_byte((uint8_t*)0);
#endif
  
  init_tone_detector();
  sei();
  
  while (1) {
    if (new_period) {
      current_freq = calculate_frequency(period_ticks);
      new_period = false;
      
      if (current_freq < MIN_VALID_FREQ) {
	OUTPUT_PORT &= ~(1 << OUTPUT_PIN);
      } else if (freq_in_range(current_freq, FREQUENCY, BANDWIDTH_PERCENT)) {
	OUTPUT_PORT |= (1 << OUTPUT_PIN);
      } else {
	OUTPUT_PORT &= ~(1 << OUTPUT_PIN);
      }
    }
  }
  
  return 0;
}
