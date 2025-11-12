/*
 * UART Console for AVR
 * Header file
 */

#ifndef UART_CONSOLE_H
#define UART_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize UART console (call once at startup) */
void console_init(void);

/* Check if character is available for reading */
int uart_available(void);

#ifdef __cplusplus
}
#endif

#endif /* UART_CONSOLE_H */
