#ifndef WHEEL_H
#define WHEEL_H

#include <stdint.h>

// Pin mapping for rotary encoder - adapt per chip

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__) || \
    defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || \
    defined(__AVR_ATmega88__) || defined(__AVR_ATmega88P__) || \
    defined(__AVR_ATmega48__) || defined(__AVR_ATmega48P__)
    // ATmega328P and similar - use INT0 on PD2
    #define WHEEL_CLK_PORT   PORTD
    #define WHEEL_CLK_DDR    DDRD
    #define WHEEL_CLK_PIN    PIND
    #define WHEEL_CLK_BIT    PD2
    
    #define WHEEL_DT_PORT    PORTD
    #define WHEEL_DT_DDR     DDRD
    #define WHEEL_DT_PIN     PIND
    #define WHEEL_DT_BIT     PD3
    
    #define WHEEL_SW_PORT    PORTD
    #define WHEEL_SW_DDR     DDRD
    #define WHEEL_SW_PIN     PIND
    #define WHEEL_SW_BIT     PD4

#elif defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega1284__) || \
      defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__)
    // ATmega1284P - use INT0 on PD2
    #define WHEEL_CLK_PORT   PORTD
    #define WHEEL_CLK_DDR    DDRD
    #define WHEEL_CLK_PIN    PIND
    #define WHEEL_CLK_BIT    PD2
    
    #define WHEEL_DT_PORT    PORTD
    #define WHEEL_DT_DDR     DDRD
    #define WHEEL_DT_PIN     PIND
    #define WHEEL_DT_BIT     PD3
    
    #define WHEEL_SW_PORT    PORTD
    #define WHEEL_SW_DDR     DDRD
    #define WHEEL_SW_PIN     PIND
    #define WHEEL_SW_BIT     PD4

#elif defined(__AVR_ATmega2560__)
    // ATmega2560 - use INT0 on PD0
    #define WHEEL_CLK_PORT   PORTD
    #define WHEEL_CLK_DDR    DDRD
    #define WHEEL_CLK_PIN    PIND
    #define WHEEL_CLK_BIT    PD0
    
    #define WHEEL_DT_PORT    PORTD
    #define WHEEL_DT_DDR     DDRD
    #define WHEEL_DT_PIN     PIND
    #define WHEEL_DT_BIT     PD1
    
    #define WHEEL_SW_PORT    PORTD
    #define WHEEL_SW_DDR     DDRD
    #define WHEEL_SW_PIN     PIND
    #define WHEEL_SW_BIT     PD2

#elif defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
    // ATtiny2313 - use PORTD (PD2/PD3/PD4)
    #define WHEEL_CLK_PORT   PORTD
    #define WHEEL_CLK_DDR    DDRD
    #define WHEEL_CLK_PIN    PIND
    #define WHEEL_CLK_BIT    PD2
    
    #define WHEEL_DT_PORT    PORTD
    #define WHEEL_DT_DDR     DDRD
    #define WHEEL_DT_PIN     PIND
    #define WHEEL_DT_BIT     PD3
    
    #define WHEEL_SW_PORT    PORTD
    #define WHEEL_SW_DDR     DDRD
    #define WHEEL_SW_PIN     PIND
    #define WHEEL_SW_BIT     PD4

#elif defined(__AVR_ATtiny13__) || defined(__AVR_ATtiny25__) || \
      defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    // ATtiny13/25/45/85 - PORTB only, default PB0/PB1/PB2
    // NOTE: Adapt pins as needed for your specific project!
    #define WHEEL_CLK_PORT   PORTB
    #define WHEEL_CLK_DDR    DDRB
    #define WHEEL_CLK_PIN    PINB
    #define WHEEL_CLK_BIT    PB0
    
    #define WHEEL_DT_PORT    PORTB
    #define WHEEL_DT_DDR     DDRB
    #define WHEEL_DT_PIN     PINB
    #define WHEEL_DT_BIT     PB1
    
    #define WHEEL_SW_PORT    PORTB
    #define WHEEL_SW_DDR     DDRB
    #define WHEEL_SW_PIN     PINB
    #define WHEEL_SW_BIT     PB2

#else
    #error "Unsupported MCU! Define wheel pins in wheel.h"
#endif

#define WHEEL_FORWARD  1
#define WHEEL_BACKWARD 2
#define WHEEL_BUTTON   3

typedef struct {
  int16_t rotation;
  int16_t mini;
  int16_t maxi;
  int8_t step;
} Wheel;

void wheel_init(int mini, int maxi, uint8_t step);
void wheel_reset(int mini, int maxi);
int wheel_get_position(void);

// User must implement these callbacks
extern void wheel_position(int position);
extern void wheel_button(int position);

#endif
