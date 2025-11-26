#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/io.h>
#include <util/delay.h>
#include <ssd1680.h>
#include <petscii.h>
#include <font-transform.h>

SSD1680 display;

int main(void) {
    SSD1680 display;
    display.width = SSD1680_WIDTH;
    display.height = SSD1680_HEIGHT;
    
    ssd1680_init();
    _delay_ms(1000);
    
    ssd1680_clear();
    ssd1680_refresh();
    _delay_ms(1000);
    
    // Same calls as before but using new function
    ssd1680_draw_string(&display, 1, 10, font8x8_high, petscii_to_screen,
                       0, 0, 0, 0, COLOR_BLACK, "COMMODORE 64");
    ssd1680_draw_string(&display, 3, 10, font8x8_high, petscii_to_screen,
                       0, 0, 0, 0, COLOR_BLACK, "BASIC V2");
    
    ssd1680_refresh();
    
    while(1);
    return 0;
}
