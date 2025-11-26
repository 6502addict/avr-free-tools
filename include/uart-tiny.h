// uart_tiny.h
#ifndef UART_TINY_H
#define UART_TINY_H

// UART configuration
#define BIT_DELAY_US (1000000UL / BAUD)
#define BIT_DELAY()     _delay_us(BIT_DELAY_US)
#define BIT_DELAY_1_5() _delay_us(BIT_DELAY_US + BIT_DELAY_US/2)

#if defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
    #define TX_PORT PORTD
    #define TX_DDR  DDRD
    #define TX_PIN  PD1
    #define RX_PORT PORTD
    #define RX_PINS PIND
    #define RX_PIN  PD0

#elif defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    #define TX_PORT PORTB
    #define TX_DDR  DDRB
    #define TX_PIN  PB2
    #define RX_PORT PORTB
    #define RX_PINS PINB
    #define RX_PIN  PB3

#else
    #error "No chip defined! Uncomment one CHIP_* definition in uart-tiny.h"
#endif 

// Function prototypes
void    uart_init(void);
void    uart_tx(uint8_t data);
uint8_t uart_rx(void);

#endif // UART_TINY_H



