// uart_tiny.h
#ifndef UART_TINY_H
#define UART_TINY_H

// UART configuration
//#define BAUD_RATE 9600
#define BIT_DELAY_US (1000000UL / BAUD_RATE)

// Pin definitions per chip
#if defined(attiny13)
    #define TX_PORT PORTB
    #define TX_DDR  DDRB
    #define TX_PIN  PB3
    #define RX_PORT PORTB
    #define RX_PINS PINB
    #define RX_PIN  PB4

#elif defined(attiny2313)
    #define TX_PORT PORTD
    #define TX_DDR  DDRD
    #define TX_PIN  PD1
    #define RX_PORT PORTD
    #define RX_PINS PIND
    #define RX_PIN  PD0

#elif defined(attiny25) || defined(attiny45) || defined(attiny85)
    #define TX_PORT PORTB
    #define TX_DDR  DDRB
    #define TX_PIN  PB3
    #define RX_PORT PORTB
    #define RX_PINS PINB
    #define RX_PIN  PB4

#else
    #error "No chip defined! Uncomment one CHIP_* definition in uart_tiny.h"
#endif 

// Function prototypes
void uart_init(void);
void uart_tx(uint8_t data);
uint8_t uart_rx(void);

#endif // UART_TINY_H



