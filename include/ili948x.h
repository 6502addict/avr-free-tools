#ifndef ILI948X_H
#define ILI948X_H

#include <stdint.h>

// Display dimensions
#define ILI948X_WIDTH   480
#define ILI948X_HEIGHT  320

// Color definitions (16-bit RGB565)
#define COLOR_BLACK   0x0000
#define COLOR_BLUE    0x001F
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_CYAN    0x07FF
#define COLOR_MAGENTA 0xF81F
#define COLOR_YELLOW  0xFFE0
#define COLOR_WHITE   0xFFFF

// LCD Pin Definitions (ATmega328P)
#define LCD_CTRL_PORT   PORTC
#define LCD_CTRL_DDR    DDRC
#define LCD_RD_PIN      PC0
#define LCD_WR_PIN      PC1
#define LCD_RS_PIN      PC2
#define LCD_CS_PIN      PC3
#define LCD_RST_PIN     PC4

// Data pins split across PORTB and PORTD
#define PORTB_DATA_MASK ((1 << PB0) | (1 << PB1))
#define PORTD_DATA_MASK ((1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7))

// Display structure
typedef struct {
    uint16_t width;
    uint16_t height;
} ILI948X;

// Initialization and basic functions
void ili948x_init(ILI948X *display);
void ili948x_fill_screen(ILI948X *display, uint16_t color);
void ili948x_set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
uint16_t ili948x_read_id(void);

// Low-level access (if needed)
void ili948x_write_command(uint8_t cmd);
void ili948x_write_data(uint8_t data);
void ili948x_write_data_16(uint16_t data);

// Drawing functions
void ili948x_draw_block8x8(ILI948X *display, uint8_t x, uint8_t y, const uint8_t *block, uint16_t color, uint16_t bg);

void ili948x_draw_string_8x8(ILI948X *display, uint16_t x, uint16_t y,
                              const uint8_t *font,
                              uint8_t (*ascii_to_index)(char),
                              uint8_t double_width, uint8_t double_height,
                              uint8_t rotation, uint8_t invert,
                              uint16_t color, uint16_t bg,
                              const char *s);

#endif
