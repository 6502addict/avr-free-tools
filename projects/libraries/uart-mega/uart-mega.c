#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "uart-mega.h"

// Local echo flag
static uint8_t echo_enabled = 1;  // Default: echo on

// Set local echo on/off
void uart_set_echo(uint8_t enable) {
  echo_enabled = enable;
}

// Initialize UART with specified baud rate
void uart_init(uint32_t baud) {
  uint16_t ubrr = (F_CPU / (16UL * baud)) - 1;
  
  // Set baud rate
  UBRR0H = (uint8_t)(ubrr >> 8);
  UBRR0L = (uint8_t)ubrr;
  
  // Enable receiver and transmitter
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  
  // Set frame format: 8 data bits, 1 stop bit, no parity
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Send a character
void uart_putc(char c) {
  // Wait for empty transmit buffer
  while (!(UCSR0A & (1 << UDRE0)));
  
  // Put data into buffer, sends the data
  UDR0 = c;
}

// Receive a character (blocking)
char uart_getc(void) {
  char c;
  
  // Wait for data to be received
  while (!(UCSR0A & (1 << RXC0)));
  
  // Get received data
  c = UDR0;
  
  // Echo back if enabled
  if (echo_enabled) {
    uart_putc(c);
  }
  
  return c;
}

// Check if data is available
uint8_t uart_available(void) {
    return (UCSR0A & (1 << RXC0)) ? 1 : 0;
}

// Send a string
void uart_puts(const char *str) {
  while (*str) {
    uart_putc(*str++);
  }
}

// Read a fixed block of data (blocking)
void uart_read_block(uint8_t *buffer, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    buffer[i] = uart_getc();
  }
}

// Write a fixed block of data
void uart_write_block(const uint8_t *buffer, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    uart_putc(buffer[i]);
  }
}

// stdio support
static int uart_putchar(char c, FILE *stream) {
  (void) stream;
  if (c == '\n') {
    uart_putc('\r');
  }
  uart_putc(c);
  return 0;
}

static int uart_getchar(FILE *stream) {
  (void) stream;
  return uart_getc();
}

static FILE uart_stream = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
static FILE *old_stdout = NULL;
static FILE *old_stdin  = NULL;

// Initialize console with stdio redirection
void uart_console() {
  old_stdout = stdout;
  old_stdin  = stdin;
  stdout     = &uart_stream;
  stdin      = &uart_stream;
}

void uart_restore() {
  stdout = old_stdout;
  stdin  = old_stdin;
}
