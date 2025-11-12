#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdio.h>
#include "uart_console.h"

#define F_CPU 16000000UL
#define DEBUG false

// Configuration
#define FREQUENCY 1000      
#define BANDWIDTH_PERCENT 10     
#define MIN_VALID_FREQ 100       

// Input pin: PB0 (Arduino pin 8 - ICP1)
#define INPUT_DDR DDRB
#define INPUT_PIN PB0

// Output pin: PB5 (Arduino pin 13 - built-in LED)
#define OUTPUT_DDR  DDRB
#define OUTPUT_PORT PORTB
#define OUTPUT_PIN  PB5

// Frequency measurement variables
volatile uint16_t period_cycles   = 0;
volatile bool     new_period      = false;
volatile uint16_t last_capture    = 0;
volatile uint32_t interrupt_count = 0;

// Initialize Timer1 Input Capture for frequency measurement
void init_frequency_meter(void) {
    INPUT_DDR &= ~(1 << INPUT_PIN);
    TCCR1A = 0;
    TCCR1B = (1 << CS11);  
    TCCR1B |= (1 << ICES1);
    TIMSK1 = (1 << ICIE1);
    TCNT1 = 0;
    last_capture = 0;
}

// Timer1 Input Capture ISR
ISR(TIMER1_CAPT_vect) {
    uint16_t current_capture = ICR1;

    period_cycles = current_capture - last_capture;
    last_capture = current_capture;
    new_period = true;
    interrupt_count++;
}

// Calculate frequency from period (returns frequency in Hz)
uint16_t calculate_frequency(uint16_t cycles) {
    if (cycles == 0) return 0;
    
    // Clock is F_CPU/8 = 2MHz with prescaler 8
    // Frequency = 2000000 / cycles
    uint32_t freq = 2000000UL / cycles;
    return (uint16_t)freq;
}

// Check if frequency is within detection band
bool freq_in_range(uint16_t measured, uint16_t target, uint8_t bandwidth_pct) {
    uint16_t bandwidth = (target * bandwidth_pct) / 100;
    uint16_t min_freq = target - (bandwidth / 2);
    uint16_t max_freq = target + (bandwidth / 2);
    return (measured >= min_freq && measured <= max_freq);
}

int main(void) {
  uint16_t current_freq = 0;
  uint32_t last_int_count = 0;

  if (DEBUG) 
    console_init();
  
  OUTPUT_DDR |= (1 << OUTPUT_PIN);
  OUTPUT_PORT &= ~(1 << OUTPUT_PIN);  // Start LOW

  init_frequency_meter();
  
  sei();
  
  // Calculate bandwidth limits (using integers)
  uint16_t bandwidth_hz = (FREQUENCY * BANDWIDTH_PERCENT) / 100;
  uint16_t freq_min = FREQUENCY - (bandwidth_hz / 2);
  uint16_t freq_max = FREQUENCY + (bandwidth_hz / 2);
  
  if (DEBUG) {
    printf("\n=== NE567 Emulator (AVR) ===\n");
    printf("Input: Pin 6 (PD6/ICP1)\n");
    printf("Output: Pin 13 (PB5)\n");
    printf("Reference frequency: %u Hz\n", FREQUENCY);
    printf("Bandwidth: %u%% (%u Hz)\n", BANDWIDTH_PERCENT, bandwidth_hz);
    printf("Detection range: %u Hz to %u Hz\n\n", freq_min, freq_max);
  }  
  
  // Main detection loop
  while (1) {
    if (interrupt_count - last_int_count > 1000) {
      if (DEBUG)
	printf("Interrupts: %lu (period_cycles=%u)\n", interrupt_count, period_cycles);
      last_int_count = interrupt_count;
    }
    
    if (new_period) {
      current_freq = calculate_frequency(period_cycles);
      new_period = false;
      
      if (current_freq < MIN_VALID_FREQ) {
	OUTPUT_PORT &= ~(1 << OUTPUT_PIN);  
      } else if (freq_in_range(current_freq, FREQUENCY, BANDWIDTH_PERCENT)) {
	OUTPUT_PORT |= (1 << OUTPUT_PIN);
	if (DEBUG)
	  printf("DETECTED: %u Hz (cycles=%u)\n", current_freq, period_cycles);
      } else {
	OUTPUT_PORT &= ~(1 << OUTPUT_PIN);
	if (DEBUG)
	  printf("Freq: %u Hz (cycles=%u)\n", current_freq, period_cycles);
      }
    }
  }
  
  return 0;
}
