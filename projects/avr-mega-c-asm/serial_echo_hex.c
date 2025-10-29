// serial_hex.c
#include <avr/io.h>
#include <util/setbaud.h>

extern void bin_to_hex(unsigned char value, unsigned char* hex_high, unsigned char* hex_low);

void uart_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    
#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif
    
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_putc(unsigned char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

unsigned char uart_getc(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

void uart_puts(const char* str) {
    while (*str) {
        uart_putc(*str++);
    }
}

int main(void) {
    unsigned char received_byte;
    unsigned char hex_high, hex_low;
    unsigned char byte_count = 0;
    
    uart_init();
    uart_puts("\r\nHex Display Ready\r\n");
    
    while (1) {
        received_byte = uart_getc();
        bin_to_hex(received_byte, &hex_high, &hex_low);
        
        uart_putc(hex_high);
        uart_putc(hex_low);
        
        byte_count++;
        
        if (byte_count >= 8) {
            uart_putc('\r');
            uart_putc('\n');
            byte_count = 0;
        } else {
            uart_putc(' ');
        }
    }
    
    return 0;
}
