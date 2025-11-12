#ifndef UART_H
#define UART_H

// Default baud rate
#ifndef UART_BAUD
#define UART_BAUD 9600
#endif

// Function prototypes
void uart_init(void);
void uart_putc(char c);
char uart_getc(void);
uint8_t uart_available(void);
void uart_puts(const char *str);

#endif
