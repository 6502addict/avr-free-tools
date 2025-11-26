#include <stdio.h>
#include <avr/io.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include <uart-mega.h>
#include <wheel.h>
#include <mcp41xxx.h>
#include <ssd1306.h>
#include <i2c.h>
#include <font-transform.h>
#include <petscii-chars.h>
#include <petscii.h>


MCP41 pot;
SSD1306 display;


static void uart_print_int(int value) {
  char buffer[8];
  snprintf(buffer, sizeof(buffer), "%d", value);
  uart_puts(buffer);
}

// Progress bar using PETSCII full blocks
void draw_progress_bar(SSD1306 *display, uint8_t x, uint8_t y, 
                       uint8_t width_chars, uint8_t value, uint8_t max) {
    uint8_t filled = (value * width_chars) / max;
    char bar[17];
    
    if (width_chars > 16) width_chars = 16;
    
    // Build bar: 0xA0 = inverted space (solid), 0x20 = space (empty)
    for (uint8_t i = 0; i < width_chars; i++) {
        bar[i] = (i < filled) ? 0xA0 : 0x20;  // Use bit 7 for invert!
    }
    bar[width_chars] = '\0';
    
    // Single draw call, no mapping needed
    ssd1306_draw_string(display, x, y, font8x8_high, NULL, 
                       0, 0, ROTATE_0, 0, bar);
}

// Called when encoder position changes
void wheel_position(int position) {
  char buffer[32];
  
  mcp41_set_wiper(&pot, (uint8_t)position);
  ssd1306_erase(&display, ERASE_BLACK);
  ssd1306_draw_string(&display, 0, 0, font8x8_high, petscii_to_screen, 0, 0, ROTATE_0, 0, "MCP41100");
  snprintf(buffer, sizeof(buffer), "POS: %d", (int) position);
  ssd1306_draw_string(&display, 0, 10, font8x8_high, petscii_to_screen, 0, 0, ROTATE_0, 0, buffer);
  draw_progress_bar(&display, 0, 20, 15, position, 255);
  ssd1306_refresh(&display);
  uart_puts("Position: ");
  uart_print_int(position);
  uart_puts("\r\n");
}

// Called when button is pressed
void wheel_button(int position) {
  (void) position;
  uart_puts("Button pressed - Reset to center\r\n");
  wheel_reset(0, 255);
  mcp41_set_wiper(&pot, MCP41_MID);
}

int main(void) {
  uint8_t buffer[1024];    
  uint8_t button_pressed = 0;
  i2c_init();
  _delay_ms(100);
  uart_init(9600);
  _delay_ms(100);
  
  
  ssd1306_init(&display, 0x3C, 128, 32, buffer);
  ssd1306_erase(&display, ERASE_BLACK);
  
  uart_puts("\r\n=== MCP41100 + Rotary Encoder ===\r\n");
  uart_puts("Turn encoder to adjust pot (0-255)\r\n");
  uart_puts("Press button to reset to center\r\n\r\n");
  

  // Initialize MCP41100 on PB2
  mcp41_init(&pot, PB2);
  
  // Initialize encoder with range 0-255
  wheel_init(0, 255, 2);
  
  uart_puts("Ready!\r\n");


  // Draw text
  ssd1306_draw_string(&display, 0, 0, font8x8_high, petscii_to_screen, 0, 0, ROTATE_0, 0, "MCP41100");
  ssd1306_refresh(&display);
  
  // Poll button in main loop
  while(1) {
    if (!(WHEEL_SW_PIN & (1 << WHEEL_SW_BIT))) {
      if (!button_pressed) {
        button_pressed = 1;
        wheel_button(wheel_get_position());
      }
    } else {
      button_pressed = 0;
    }
    _delay_ms(50);
  }
  
  return 0;
}
