#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include <i2c.h>
#include <ssd1306.h>
#include <font-transform.h>
#include <petscii.h>

// ADC functions
static void adc_init(void) {
    // REFS0 = 1: AVcc reference
    // ADLAR = 1: Left adjust for 8-bit reading
    ADMUX = (1 << REFS0) | (1 << ADLAR);
    
    // Enable ADC, prescaler 128
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    // Dummy read
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
}

static uint8_t adc_read_8bit(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADCH;
}

int main(void) {
    SSD1306 display;
    uint8_t buffer[512];  // 128x32 = 512 bytes
    uint8_t x_val, y_val;
    char text_buffer[16];
    
    // Initialize
    i2c_init();
    ssd1306_init(&display, 0x3C, 128, 32, buffer);
    adc_init();
    
    while(1) {
        // Read both axes
        x_val = adc_read_8bit(0);  // PC0
        y_val = adc_read_8bit(1);  // PC1
        
        // Clear display
        ssd1306_erase(&display, ERASE_BLACK);
        
        // Display X axis
        ssd1306_draw_string(&display, 0, 0, font8x8_high, petscii_to_screen, 
                           0, 0, ROTATE_0, 0, "X:");
        snprintf(text_buffer, sizeof(text_buffer), "%3d", x_val);
        ssd1306_draw_string(&display, 16, 0, font8x8_high, petscii_to_screen, 
                           0, 0, ROTATE_0, 0, text_buffer);
        
        // Display Y axis
        ssd1306_draw_string(&display, 0, 16, font8x8_high, petscii_to_screen, 
                           0, 0, ROTATE_0, 0, "Y:");
        snprintf(text_buffer, sizeof(text_buffer), "%3d", y_val);
        ssd1306_draw_string(&display, 16, 16, font8x8_high, petscii_to_screen, 
                           0, 0, ROTATE_0, 0, text_buffer);
        
        // Refresh display
        ssd1306_refresh(&display);
        
        _delay_ms(100);
    }
    
    return 0;
}
