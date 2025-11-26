#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <ssd1306.h>
#include <i2c.h>
#include <font-transform.h>
#include <petscii.h>

int main(void) {
    SSD1306 display;
    uint8_t buffer[1024];
    
    i2c_init();
    _delay_ms(100);
    
    ssd1306_init(&display, 0x3C, 128, 32, buffer);
    ssd1306_erase(&display, ERASE_BLACK);
    
    // Draw text 
    ssd1306_draw_string(&display, 0, 0,  font8x8_low, petscii_to_screen, 0, 0, ROTATE_0, 0, "COMMODORE 64");
    ssd1306_draw_string(&display, 0, 8,  font8x8_low, petscii_to_screen, 0, 0, ROTATE_0, 0, "BASIC V2");
    ssd1306_draw_string(&display, 0, 16, font8x8_low, petscii_to_screen, 0, 0, ROTATE_0, 0, "64K RAM SYSTEM");
    ssd1306_draw_string(&display, 0, 24, font8x8_low, petscii_to_screen, 0, 0, ROTATE_0, 0, "READY.");
    
    ssd1306_refresh(&display);
    
    while(1);
    return 0;
}
