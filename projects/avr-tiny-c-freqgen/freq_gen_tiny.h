// freq_gen_tiny.h
#ifndef FREQ_GEN_TINY_H
#define FREQ_GEN_TINY_H

// Output pin: PB0 (OC0A on ATtiny45/85)
#define FREQ_OUTPUT_PIN PB0

void freq_gen_init(uint16_t frequency);
void freq_gen_set(uint16_t frequency);
void freq_gen_stop(void);

#endif
