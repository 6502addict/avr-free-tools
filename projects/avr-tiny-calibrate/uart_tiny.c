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
  BIT_DELAY();
  
  // 8 data bits
  for (uint8_t i = 8; i; i--) {
    if (data & 0x01) {
      TX_PORT |= (1 << TX_PIN);
    } else {
      TX_PORT &= ~(1 << TX_PIN);
    }
    BIT_DELAY();
    data >>= 1;
  }
    
  // Stop bit
  TX_PORT |= (1 << TX_PIN);
  BIT_DELAY();
}

uint8_t uart_rx(void) {
    uint8_t data = 0;
    
    // Wait for start bit
    while (RX_PINS & (1 << RX_PIN));
    
    // Wait to middle of first data bit, compensate for loop overhead
    _delay_us(BIT_DELAY_US + BIT_DELAY_US/2 - 7);
    
    // Read 8 data bits - unroll to eliminate loop overhead
    if (RX_PINS & (1 << RX_PIN)) data |= 0x01;
    _delay_us(BIT_DELAY_US - 7);  // Subtract ~5µs for execution
    
    if (RX_PINS & (1 << RX_PIN)) data |= 0x02;
    _delay_us(BIT_DELAY_US - 7);
    
    if (RX_PINS & (1 << RX_PIN)) data |= 0x04;
    _delay_us(BIT_DELAY_US - 7);
    
    if (RX_PINS & (1 << RX_PIN)) data |= 0x08;
    _delay_us(BIT_DELAY_US - 7);
    
    if (RX_PINS & (1 << RX_PIN)) data |= 0x10;
    _delay_us(BIT_DELAY_US - 7);
    
    if (RX_PINS & (1 << RX_PIN)) data |= 0x20;
    _delay_us(BIT_DELAY_US - 7);
    
    if (RX_PINS & (1 << RX_PIN)) data |= 0x40;
    _delay_us(BIT_DELAY_US - 7);
    
    if (RX_PINS & (1 << RX_PIN)) data |= 0x80;
    
    _delay_us(BIT_DELAY_US);  // Stop bit
    
    return data;
}



/*
uint8_t uart_rx(void) {
    uint8_t data = 0;
    
    // Wait for start bit
    while (RX_PINS & (1 << RX_PIN));
    
    // Compensate for loop exit + function call overhead (typically 5-10µs)
    _delay_us(BIT_DELAY_US + BIT_DELAY_US/2 - 8);  // Subtract ~8µs
    
    // Read 8 data bits
    for (uint8_t i = 8; i; i--) {
        data >>= 1;
        if (RX_PINS & (1 << RX_PIN)) {
            data |= 0x80;
        }
        _delay_us(BIT_DELAY_US);
    }
    
    _delay_us(BIT_DELAY_US);
    return data;
}
*/

/*
uint8_t uart_rx(void) {
  uint8_t data = 0;
  
  // Wait for start bit
  while (RX_PINS & (1 << RX_PIN));
  BIT_DELAY_1_5();
  // Read 8 data bits
  for (uint8_t i = 8; i; i--) {
    data >>= 1;
    if (RX_PINS & (1 << RX_PIN)) {
      data |= 0x80;
    }
    BIT_DELAY();
  }
  BIT_DELAY();    
  return data;
}
*/
