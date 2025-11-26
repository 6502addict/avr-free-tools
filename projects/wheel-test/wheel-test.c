#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <uart-mega.h>
#include <wheel.h>

static void uart_print_int(int value) {
  char buffer[8];
  snprintf(buffer, sizeof(buffer), "%d", value);
  uart_puts(buffer);
}

void wheel_position(int position) {
  uart_puts("Position: ");
  uart_print_int(position);
  uart_puts("\r\n");
}

void wheel_button(int position) {
  uart_puts("Button at: ");
  uart_print_int(position);
  uart_puts("\r\n");
}

int main(void) {
  uint8_t button_pressed = 0;  // Add this line
  
  uart_init(9600);
  _delay_ms(100);
  
  uart_puts("\r\n=== Rotary Encoder Test ===\r\n");
  
  wheel_init(0, 255, 2);
  
  uart_puts("Ready\r\n");
  
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
