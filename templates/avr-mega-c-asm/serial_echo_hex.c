// serial_echo_hex.c
#include <avr/io.h>
#include <stdio.h>
#include <uart-mega.h>

extern void bin_to_hex(unsigned char value, unsigned char* hex_high, unsigned char* hex_low);

int main(void) {
    unsigned char received_byte;
    unsigned char hex_high, hex_low;
    unsigned char byte_count = 0;
    
    console_init(BAUD);
    printf("\r\nHex Display Ready\r\n");
    
    while (1) {
        received_byte = getchar();
        bin_to_hex(received_byte, &hex_high, &hex_low);
        
        putchar(hex_high);
        putchar(hex_low);
        
        byte_count++;
        
        if (byte_count >= 8) {
            putchar('\r');
            putchar('\n');
            byte_count = 0;
        } else {
            putchar(' ');
        }
    }
    
    return 0;
}
