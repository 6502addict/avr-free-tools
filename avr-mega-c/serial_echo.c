#include <avr/io.h>
#include <util/setbaud.h>

// UART initialization - works for all supported chips
void uart_init(void) {
    // Set baud rate
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    
#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif
    
    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    
    // Set frame format: 8 data bits, 1 stop bit, no parity
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

int main(void) {
    unsigned char received_byte;
    
    // Initialize UART
    uart_init();
    
    // Main loop - echo received data
    while (1) {
        received_byte = uart_getc();
        uart_putc(received_byte);
    }
    
    return 0;
}
