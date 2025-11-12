// uart_echo_tiny13.c
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "uart_tiny.h"

void uart_init(void) {
    TX_DDR |= (1 << TX_PIN);      // TX as output
    TX_PORT |= (1 << TX_PIN);     // TX idle high
    PORTB |= (1 << RX_PIN);       // RX pullup
}

void uart_tx(uint8_t data) {
    // Start bit
    TX_PORT &= ~(1 << TX_PIN);
    _delay_us(BIT_DELAY_US);
    
    // 8 data bits
    for (uint8_t i = 8; i; i--) {
        if (data & 0x01) {
            TX_PORT |= (1 << TX_PIN);
        } else {
            TX_PORT &= ~(1 << TX_PIN);
        }
        _delay_us(BIT_DELAY_US);
        data >>= 1;
    }
    
    // Stop bit
    TX_PORT |= (1 << TX_PIN);
    _delay_us(BIT_DELAY_US);
}

uint8_t uart_rx(void) {
    uint8_t data = 0;
    
    // Wait for start bit
    while (RX_PINS & (1 << RX_PIN));
    _delay_us(BIT_DELAY_US + BIT_DELAY_US/2);
    
    // Read 8 data bits
    for (uint8_t i = 8; i; i--) {
        data >>= 1;
        if (RX_PINS & (1 << RX_PIN)) {
            data |= 0x80;
        }
        _delay_us(BIT_DELAY_US);
    }
    
    _delay_us(BIT_DELAY_US);  // Stop bit
    
    return data;
}
