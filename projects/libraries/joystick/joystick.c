#include <avr/io.h>
#include <stdint.h>
#include "joystick.h"

// ADC channel definitions
#define ADC_CHANNEL_X  0  // PC0/ADC0
#define ADC_CHANNEL_Y  1  // PC1/ADC1
#define BUTTON_PIN     PD2

// Read ADC channel and return 0-255
static uint8_t adc_read_8bit(uint8_t channel) {
    // Select ADC channel (0-7)
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    
    // Start conversion
    ADCSRA |= (1 << ADSC);
    
    // Wait for conversion to complete
    while (ADCSRA & (1 << ADSC));
    
    // Read 10-bit result and scale to 8-bit (0-255)
    // ADC is 10-bit (0-1023), we want 8-bit (0-255)
    // Simply read ADCH which contains the upper 8 bits
    return ADCH;
}

void joystick_init(void) {
    // Set ADC pins as input (already default, but explicit)
    DDRC &= ~((1 << PC0) | (1 << PC1));
    
    // Set button pin as input with pull-up
    DDRD &= ~(1 << BUTTON_PIN);
    PORTD |= (1 << BUTTON_PIN);
    
    // Configure ADC
    // REFS0 = 1: Use AVcc as reference (5V)
    // ADLAR = 1: Left adjust result (so we can read ADCH for 8-bit)
    ADMUX = (1 << REFS0) | (1 << ADLAR);
    
    // Enable ADC, set prescaler to 128 (16MHz / 128 = 125kHz ADC clock)
    // ADC needs 50-200kHz for best accuracy
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    // Do a dummy read to initialize ADC
    adc_read_8bit(0);
}

uint8_t joystick_read_x(void) {
    return adc_read_8bit(ADC_CHANNEL_X);
}

uint8_t joystick_read_y(void) {
    return adc_read_8bit(ADC_CHANNEL_Y);
}

uint8_t joystick_read_button(void) {
    // Button is active LOW (pressed = 0, released = 1)
    return !(PIND & (1 << BUTTON_PIN));
}
