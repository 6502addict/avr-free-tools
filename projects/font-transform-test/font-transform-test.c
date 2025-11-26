#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <ssd1306.h>
#include <i2c.h>
#include <font-transform.h>
#include <petscii.h>

SSD1306 display;
uint8_t buffer[1024];

int main(void) {
  i2c_init();
  ssd1306_init(&display, 0x3C, 128, 32, buffer);
  ssd1306_erase(&display, ERASE_BLACK);
  

  // Normal horizontal text
  ssd1306_erase(&display, ERASE_BLACK);
  ssd1306_draw_string(&display, 0, 0, font8x8_low, petscii_to_screen, 0, 0, ROTATE_0, 0, "HELLO");  
  ssd1306_refresh(&display);
  _delay_ms(5000);
  
  // Double size horizontal
  ssd1306_erase(&display, ERASE_BLACK);
  ssd1306_draw_string(&display, 0, 0, font8x8_low, petscii_to_screen, 1, 1, ROTATE_0, 0, "BIG");
  ssd1306_refresh(&display);
  _delay_ms(5000);
  
  // Vertical downward
  ssd1306_erase(&display, ERASE_BLACK);
  ssd1306_draw_string(&display, 0, 0, font8x8_low, petscii_to_screen, 0, 0, ROTATE_90, 0, "DOWN");
  ssd1306_refresh(&display);
  _delay_ms(5000);

  // Vertical upward (start at bottom!)
  ssd1306_erase(&display, ERASE_BLACK);
  ssd1306_draw_string(&display, 0, 24, font8x8_low, petscii_to_screen, 0, 0, ROTATE_270, 0, "UP");
  ssd1306_refresh(&display);
  _delay_ms(5000);
  
  // NULL for direct ASCII (no mapping)
  ssd1306_erase(&display, ERASE_BLACK);
  ssd1306_draw_string(&display, 0, 0, font8x8_low, NULL, 1, 1, ROTATE_0, 1, "ASCII");
  ssd1306_refresh(&display);
  
  while(1);
  return 0;
}
