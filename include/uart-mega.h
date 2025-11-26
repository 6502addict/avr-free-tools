#ifndef UART_CONSOLE_H
#define UART_CONSOLE_H

// Function prototypes
void    uart_init(uint32_t);
void    uart_putc(char);
char    uart_getc(void);
uint8_t uart_available(void);
void    uart_puts(const char *);
void    uart_read_block(uint8_t *, uint8_t);
void    uart_read_block(uint8_t *, uint8_t);
void    uart_set_echo(uint8_t);  // Enable/disable local echo
void    uart_console();
void    uart_restore();

#endif
