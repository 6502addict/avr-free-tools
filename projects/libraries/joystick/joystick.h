#ifndef JOYSTICK_H
#define JOYSTICK_H

// Initialize ADC for joystick reading
void joystick_init(void);

// Read X axis (returns 0-255)
uint8_t joystick_read_x(void);

// Read Y axis (returns 0-255)
uint8_t joystick_read_y(void);

// Read button (returns 1 if pressed)
uint8_t joystick_read_button(void);

#endif
