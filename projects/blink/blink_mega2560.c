#include <avr/io.h>
#include <timer.h>

// LED on Arduino Mega is PB7 (pin 13)
#define LED_DDR  DDRB
#define LED_PORT PORTB
#define LED_PIN  PB7

int main(void) {
    // Set LED pin as output
    LED_DDR |= (1 << LED_PIN);
    
    // Blink at 1Hz (500ms on, 500ms off)
    while (1) {
        LED_PORT |= (1 << LED_PIN);   // LED on
        timer_delay_ms(500);
        
        LED_PORT &= ~(1 << LED_PIN);  // LED off
        timer_delay_ms(500);
    }
    
    return 0;
}
