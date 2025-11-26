#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

// Configuration
#define MODE_ASTABLE    0
#define MODE_MONOSTABLE 1
#define MODE_BISTABLE   2

#define OPERATION_MODE MODE_ASTABLE

// Pin definitions
#define OUTPUT_PIN PB0
#define TRIGGER_PIN PB1
#define RESET_PIN PB2

// Timing
#define FREQ_HZ  1200
#define PULSE_MS 1000

// Globals
volatile uint16_t overflow_count = 0;
volatile uint16_t overflow_target = 0;
volatile uint8_t pulse_active = 0;
volatile uint16_t pulse_counter = 0;

// Timer overflow ISR - handles both astable and monostable
#if OPERATION_MODE == MODE_MONOSTABLE
ISR(TIM0_OVF_vect) {
  if (pulse_active) {
    if (--pulse_counter == 0) {
      PORTB &= ~(1 << OUTPUT_PIN);
      pulse_active = 0;
    }
  }
}
#endif

// Pin change interrupt
ISR(PCINT0_vect) {
#if OPERATION_MODE == MODE_MONOSTABLE
  if (!(PINB & (1 << TRIGGER_PIN)) && !pulse_active) {
    PORTB |= (1 << OUTPUT_PIN);
    pulse_active = 1;
    pulse_counter = (PULSE_MS * F_CPU) / (1000UL * 1024 * 256);
    TCNT0 = 0;
  }
#endif
}

void timer_astable_init() {
  uint32_t ocr_value;
  uint8_t prescaler_bits;
  
  // Select prescaler to keep OCR in 0-255 range
#if FREQ_HZ >= (F_CPU / (2UL * 1 * 256))
  // Prescaler 1: high frequencies (>15kHz @ 8MHz)
  ocr_value = (F_CPU / (2UL * 1 * FREQ_HZ)) - 1;
  prescaler_bits = (1 << CS00);
#elif FREQ_HZ >= (F_CPU / (2UL * 8 * 256))
  // Prescaler 8: (1.9kHz - 15kHz @ 8MHz)
  ocr_value = (F_CPU / (2UL * 8 * FREQ_HZ)) - 1;
  prescaler_bits = (1 << CS01);
#elif FREQ_HZ >= (F_CPU / (2UL * 64 * 256))
  // Prescaler 64: (244Hz - 1.9kHz @ 8MHz)
  ocr_value = (F_CPU / (2UL * 64 * FREQ_HZ)) - 1;
  prescaler_bits = (1 << CS01) | (1 << CS00);
#elif FREQ_HZ >= (F_CPU / (2UL * 256 * 256))
  // Prescaler 256: (61Hz - 244Hz @ 8MHz)
  ocr_value = (F_CPU / (2UL * 256 * FREQ_HZ)) - 1;
  prescaler_bits = (1 << CS02);
#else
  // Prescaler 1024: low frequencies (<61Hz @ 8MHz)
  ocr_value = (F_CPU / (2UL * 1024 * FREQ_HZ)) - 1;
  prescaler_bits = (1 << CS02) | (1 << CS00);
#endif
  
  if (ocr_value > 255) ocr_value = 255;
  
  TCCR0A = (1 << COM0A0) | (1 << WGM01);  // Toggle OC0A, CTC
  TCCR0B = prescaler_bits;
  OCR0A = ocr_value;
  DDRB |= (1 << OUTPUT_PIN);
}

// Timer setup for monostable mode
void timer_monostable_init() {
  TCCR0B = (1 << CS02) | (1 << CS00);  // Prescaler 1024
#if defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny13A__)
  TIMSK0 = (1 << TOIE0);
#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__)  || defined(__AVR_ATtiny45__)
  TIMSK = (1 << TOIE0);
#endif
  DDRB |= (1 << OUTPUT_PIN);
  PORTB &= ~(1 << OUTPUT_PIN);
}

// Pin change interrupt for trigger
void trigger_init() {
  DDRB &= ~(1 << TRIGGER_PIN);
  PORTB |= (1 << TRIGGER_PIN);
  PCMSK = (1 << TRIGGER_PIN);
  GIMSK = (1 << PCIE);
}

// Bistable mode setup
void bistable_init() {
  DDRB |= (1 << OUTPUT_PIN);
  DDRB &= ~(1 << TRIGGER_PIN);
  DDRB &= ~(1 << RESET_PIN);
  PORTB |= (1 << TRIGGER_PIN) | (1 << RESET_PIN);
  PORTB &= ~(1 << OUTPUT_PIN);
}

int main(void) {
#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)  
  OSCCAL = eeprom_read_byte((uint8_t*)0);
#endif

#if OPERATION_MODE == MODE_ASTABLE
  timer_astable_init();
  sei();
  
#elif OPERATION_MODE == MODE_MONOSTABLE
  timer_monostable_init();
  trigger_init();
  sei();
  
#elif OPERATION_MODE == MODE_BISTABLE
  bistable_init();
#endif
  
  while(1) {
#if OPERATION_MODE == MODE_BISTABLE
    if (!(PINB & (1 << TRIGGER_PIN))) {
      PORTB |= (1 << OUTPUT_PIN);
    }
    if (!(PINB & (1 << RESET_PIN))) {
      PORTB &= ~(1 << OUTPUT_PIN);
    }
#endif
  }
}
