#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "timer.h"

// Timer configuration based on MCU type
#if defined(__AVR_ATmega328P__)
    #define TIMER_TCCRA  TCCR1A
    #define TIMER_TCCRB  TCCR1B
    #define TIMER_TCNT_L TCNT1L
    #define TIMER_TCNT_H TCNT1H
    #define TIMER_TIMSK  TIMSK1
    #define TIMER_CS_BITS (_BV(CS10))  // No prescaler

#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__)
    #define TIMER_TCCRA  TCCR1A
    #define TIMER_TCCRB  TCCR1B
    #define TIMER_TCNT_L TCNT1L
    #define TIMER_TCNT_H TCNT1H
    #define TIMER_TIMSK  TIMSK1
    #define TIMER_CS_BITS (_BV(CS10))  // No prescaler

#elif defined(__AVR_ATmega2560__)
    #define TIMER_TCCRA  TCCR1A
    #define TIMER_TCCRB  TCCR1B
    #define TIMER_TCNT_L TCNT1L
    #define TIMER_TCNT_H TCNT1H
    #define TIMER_TIMSK  TIMSK1
    #define TIMER_CS_BITS (_BV(CS10))  // No prescaler

#elif defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
    #define TIMER_TCCRA  TCCR1A
    #define TIMER_TCCRB  TCCR1B
    #define TIMER_TCNT_L TCNT1L
    #define TIMER_TCNT_H TCNT1H
    #define TIMER_TIMSK  TIMSK
    #define TIMER_CS_BITS (_BV(CS10))  // No prescaler

#else
    #error "Unsupported MCU - please add Timer1 definitions"
#endif

// Static flag to track if timer is running
static volatile uint8_t timer_running = 0;

/*
 * Get timer cpu speed
 * Returns: the cpu speed in MHz
 */
uint8_t timer_cpu_speed(void) {
    return (uint8_t)(F_CPU / 1000000UL);
}

/*
 * Start the timer (resets counter to 0)
 */
void timer_start(void) {
    // Stop timer
    TIMER_TCCRB = 0;
    
    // Reset counter
    TIMER_TCNT_H = 0;
    TIMER_TCNT_L = 0;
    
    // Set normal mode (no PWM)
    TIMER_TCCRA = 0;
    
    // Start timer with no prescaler (CS10=1)
    TIMER_TCCRB = TIMER_CS_BITS;
    
    timer_running = 1;
}

/*
 * Stop the timer (preserves current count)
 */
void timer_stop(void) {
    TIMER_TCCRB = 0;  // Stop clock
    timer_running = 0;
}

/*
 * Read current timer value
 * Returns: 16-bit timer count
 */
uint16_t timer_read(void) {
    uint8_t low, high;
    
    // Read low byte first, then high byte (order matters for 16-bit timer)
    low = TIMER_TCNT_L;
    high = TIMER_TCNT_H;
    
    return ((uint16_t)high << 8) | low;
}

/*
 * Get timer status
 * Returns: 1 if running, 0 if stopped
 */
uint8_t timer_is_running(void) {
    return timer_running;
}

/*
 * Precise delay using hardware timer
 * ticks: number of timer ticks to delay
 */
void timer_delay_ticks(unsigned int ticks) {
    unsigned int current_time;
    
    timer_start();

    do {
        current_time = timer_read();
    } while (current_time < ticks);
    
    timer_stop();
}

/*
 * Get timer frequency in Hz
 */
uint32_t timer_get_frequency_hz(void) {
    uint8_t cpu_mhz = timer_cpu_speed();
    return (uint32_t)cpu_mhz * 1000000UL;
}

/*
 * Get ticks per millisecond
 */
uint32_t timer_get_ticks_per_ms(void) {
    uint8_t cpu_mhz = timer_cpu_speed();
    return (uint32_t)cpu_mhz * 1000UL;
}

/*
 * Get ticks per microsecond
 */
uint16_t timer_get_ticks_per_us(void) {
    return timer_cpu_speed();
}

/*
 * Convert timer ticks to microseconds
 */
uint32_t timer_ticks_to_us(unsigned int ticks) {
    uint8_t cpu_mhz = timer_cpu_speed();
    return (uint32_t)ticks / cpu_mhz;
}

/*
 * Convert timer ticks to milliseconds using actual CPU speed
 */
uint16_t timer_ticks_to_ms(uint16_t ticks) {
    uint8_t cpu_mhz = timer_cpu_speed();
    uint32_t ticks_per_ms = (uint32_t)cpu_mhz * 1000UL;
    return (uint16_t)(ticks / ticks_per_ms);
}

/*
 * Delay in microseconds using actual CPU speed
 */
void timer_delay_us(unsigned int microseconds) {
    uint8_t cpu_mhz;
    uint32_t ticks_per_us, total_ticks;
    unsigned int ms, remaining_us;
    uint32_t remaining_ticks;
    
    cpu_mhz = timer_cpu_speed();
    ticks_per_us = cpu_mhz;
    total_ticks = ticks_per_us * microseconds;
    
    // Handle overflow for large delays
    if (total_ticks > 65535UL) {
        ms = microseconds / 1000;
        remaining_us = microseconds % 1000;
        if (ms > 0) 
            timer_delay_ms(ms);
        if (remaining_us > 0) {
            remaining_ticks = ticks_per_us * remaining_us;
            if (remaining_ticks <= 65535UL) 
                timer_delay_ticks((unsigned int)remaining_ticks);
        }
    } else {
        timer_delay_ticks((unsigned int)total_ticks);
    }
}

/*
 * Delay in milliseconds using actual CPU speed
 */
void timer_delay_ms(unsigned int milliseconds) {
    uint8_t cpu_mhz;
    uint32_t ticks_per_ms;
    uint32_t adjusted_ticks;
    unsigned int i;
    
    cpu_mhz = timer_cpu_speed();
    ticks_per_ms = (uint32_t)cpu_mhz * 1000UL;
    adjusted_ticks = ticks_per_ms - (590 / cpu_mhz);
    
    for (i = 0; i < milliseconds; i++)
        timer_delay_ticks((unsigned int)adjusted_ticks);
}

