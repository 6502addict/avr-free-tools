/*
 * UART Console for AVR
 * Provides stdio redirection to UART0
 */

#include <avr/io.h>
#include <stdio.h>

#ifndef BAUD
#define BAUD 9600
#endif

#include <util/setbaud.h>

/* UART initialization */
void uart_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    
#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif

    /* Enable transmitter and receiver */
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    
    /* 8 data bits, 1 stop bit, no parity */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

/* Send one character */
static int uart_putchar(char c, FILE *stream) {
  if (c == '\n') {
    uart_putchar('\r', stream);
  }
  
  /* Wait for empty transmit buffer */
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  
  /* Put data into buffer */
  UDR0 = c;
  
  return 0;
}

/* Receive one character */
static int uart_getchar(FILE *stream) {
  uint8_t c;

  /* Wait for data */
  while (!(UCSR0A & (1 << RXC0)))
    ;
  
  /* Get and return received data */
  c = UDR0;
  if (c == '\r')
    c = '\n';
  // Echo character back
  if (c == '\n') {
    uart_putchar('\r', stream);
    uart_putchar('\n', stream);
  } else if (c >= 32 && c < 127) {
    uart_putchar(c, stream);  // Echo printable characters
  } else if (c == 127 || c == 8) {  // Backspace
    uart_putchar('\b', stream);
    uart_putchar(' ', stream);
    uart_putchar('\b', stream);
  } 

  return c;
}

/* Check if character available */
int uart_available(void) {
    return (UCSR0A & (1 << RXC0)) ? 1 : 0;
}

/* FILE structure for stdio */
static FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

/* Initialize UART console */
void console_init(void) {
    uart_init();
    
    /* Redirect stdout and stdin to UART */
    stdout = &uart_output;
    stdin = &uart_input;
}

