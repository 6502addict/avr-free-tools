#include <avr/io.h>
#include <util/delay.h>

#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1) 

// UART initialization - works for all supported chips
void uart_init(void) {
  // Set baud rate
  
  // Write high byte to UBRR0H
  UBRR0H = (unsigned char)(UBRR_VALUE >> 8);
  // Write low byte to UBRR0L
  UBRR0L = (unsigned char)UBRR_VALUE;
  
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Transmit single byte
void uart_putc(unsigned char data) {
  // Wait for empty transmit buffer
  while (!(UCSR0A & (1 << UDRE0)));
  
  // Put data into buffer, sends the data
  UDR0 = data;
}

// Receive single byte
unsigned char uart_getc(void) {
  // Wait for data to be received
  while (!(UCSR0A & (1 << RXC0)));
  
  // Get and return received data from buffer
  return UDR0;
}



// Print a string
void uart_print(const char *str) {
  while (*str) {
    uart_putc(*str++);
  }
}


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
  uart_init();
  
  uart_print("Hello, World\n\r");
  
  
  // Main loop - echo received data
  while (1) {
    received_byte = uart_getc();
    uart_putc(received_byte);
  }

  return 0;
}
