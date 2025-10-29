#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Function prototypes - same API as original
void     timer_start(void);
void     timer_stop(void);
uint16_t timer_read(void);
uint8_t  timer_is_running(void);
uint8_t  timer_cpu_speed(void);
uint32_t timer_ticks_to_us(unsigned int ticks);
uint16_t timer_ticks_to_ms(unsigned int ticks);
void     timer_delay_ticks(unsigned int ticks);
void     timer_delay_us(unsigned int microseconds);
void     timer_delay_ms(unsigned int milliseconds);
uint32_t timer_get_frequency_hz(void);
uint32_t timer_get_ticks_per_ms(void);
uint16_t timer_get_ticks_per_us(void);

#endif // TIMER_H

