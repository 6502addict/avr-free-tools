#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <uart-mega.h>

int main(void) {
  unsigned char received_byte;
  
  // Blink built-in LED on Mega (PB7) to confirm code runs
  DDRB |= (1 << PB7);
  
  for(int i = 0; i < 15; i++) {
    PORTB |= (1 << PB7);   // LED on
    _delay_ms(100);
    PORTB &= ~(1 << PB7);  // LED off
    _delay_ms(100);
  }
  
  // Initialize UART
  console_init(BAUD);
  
  printf("echoing rx on tx\r\n");
  
  // Main loop - echo received data
  while (1) {
    received_byte = getchar();
    putchar(received_byte);
  }

  return 0;
}
