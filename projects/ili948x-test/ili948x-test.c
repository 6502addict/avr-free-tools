#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <uart-mega.h>
#include <ili948x.h>
#include <util/delay.h>
#include <font-transform.h>
#include <petscii.h>

int main(void) {
    ILI948X display;
    
    uart_init(9600);
    uart_puts("\n--- ILI9488 8x8 Font Test ---\n");
    
    DDRB |= (1 << PB5);
    
    ili948x_init(&display);
    ili948x_fill_screen(&display, COLOR_BLACK);
    
    // Test 1: Normal 8x8 text
    ili948x_draw_string_8x8(&display, 10, 10, font8x8_low, petscii_to_screen,
                           0, 0, ROTATE_0, 0, COLOR_GREEN, COLOR_BLACK,
                           "COMMODORE 64");
    
    // Test 2: Double size
    ili948x_draw_string_8x8(&display, 10, 50, font8x8_low, petscii_to_screen,
                           1, 1, ROTATE_0, 0, COLOR_YELLOW, COLOR_BLACK,
                           "BIG TEXT");
    
    // Test 3: Double width only
    ili948x_draw_string_8x8(&display, 10, 100, font8x8_low, petscii_to_screen,
                           1, 0, ROTATE_0, 0, COLOR_CYAN, COLOR_BLACK,
                           "WIDE");
    
    // Test 4: Inverted
    ili948x_draw_string_8x8(&display, 10, 130, font8x8_low, petscii_to_screen,
                           0, 0, ROTATE_0, 1, COLOR_BLACK, COLOR_WHITE,
                           "INVERTED");
    
    // Test 5: Progress bar with PETSCII graphics
    char bar[17];
    for (uint8_t i = 0; i < 10; i++) {
        bar[i] = 0xA0;  // Inverted space
    }
    for (uint8_t i = 10; i < 15; i++) {
        bar[i] = 0x20;  // Normal space
    }
    bar[15] = '\0';
    
    ili948x_draw_string_8x8(&display, 10, 160, font8x8_low, NULL,
                           0, 0, ROTATE_0, 0, COLOR_MAGENTA, COLOR_BLACK,
                           bar);
    
    // Test 6: Vertical text
    ili948x_draw_string_8x8(&display, 400, 10, font8x8_low, petscii_to_screen,
                           0, 0, ROTATE_90, 0, COLOR_RED, COLOR_BLACK,
                           "VERTICAL");
    
    uart_puts("Test complete\n");
    
    while (1) {
        PORTB ^= (1 << PB5);
        _delay_ms(500);
    }
    
    return 0;
}

