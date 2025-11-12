// freq_gen_tiny.c
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include "freq_gen_tiny.h"


void freq_gen_init(uint16_t frequency) {
    DDRB |= (1 << FREQ_OUTPUT_PIN);
    
    uint32_t ocr_value;
    uint8_t prescaler_bits;
    
    if (frequency >= 3906) {
        ocr_value = (F_CPU / (2UL * frequency)) - 1;
        prescaler_bits = (1 << CS00);
    } else if (frequency >= 489) {
        ocr_value = (F_CPU / (16UL * frequency)) - 1;
        prescaler_bits = (1 << CS01);
    } else if (frequency >= 61) {
        ocr_value = (F_CPU / (128UL * frequency)) - 1;
        prescaler_bits = (1 << CS01) | (1 << CS00);
    } else if (frequency >= 16) {
        ocr_value = (F_CPU / (512UL * frequency)) - 1;
        prescaler_bits = (1 << CS02);
    } else {
        ocr_value = (F_CPU / (2048UL * frequency)) - 1;
        prescaler_bits = (1 << CS02) | (1 << CS00);
    }
    
    if (ocr_value > 255) ocr_value = 255;
    
    TCCR0A = (1 << COM0A0) | (1 << WGM01);
    TCCR0B = prescaler_bits;
    OCR0A = ocr_value;
    TCNT0 = 0;
}

void freq_gen_set(uint16_t frequency) {
    freq_gen_stop();
    freq_gen_init(frequency);
}

void freq_gen_stop(void) {
    TCCR0B = 0;
    PORTB &= ~(1 << FREQ_OUTPUT_PIN);
}
