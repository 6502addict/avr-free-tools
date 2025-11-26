#ifndef SSD1680_H
#define SSD1680_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// Pin definitions
#define SSD1680_CS_PORT    PORTB
#define SSD1680_CS_DDR     DDRB
#define SSD1680_CS_PIN     PB2

#define SSD1680_DC_PORT    PORTD
#define SSD1680_DC_DDR     DDRD
#define SSD1680_DC_PIN     PD7

#define SSD1680_RST_PORT   PORTD
#define SSD1680_RST_DDR    DDRD
#define SSD1680_RST_PIN    PD6

#define SSD1680_BUSY_PORT  PORTD
#define SSD1680_BUSY_DDR   DDRD
#define SSD1680_BUSY_PIN   PIND
#define SSD1680_BUSY_BIT   PD5

// Display dimensions
#define SSD1680_WIDTH       122
#define SSD1680_HEIGHT      250
#define SSD1680_WIDTH_BYTE  16   // 128 pixels addressable
#define SSD1680_BUFFER_SIZE (SSD1680_WIDTH_BYTE * SSD1680_HEIGHT)  // 4000 bytes

// Colors
#define COLOR_WHITE 0
#define COLOR_BLACK 1

typedef struct {
  uint8_t width;   // 122 usable pixels
  uint8_t height;  // 250 pixels
} SSD1680;

// API functions
void ssd1680_init(void);
void ssd1680_clear(void);
void ssd1680_fill_black(void);
void ssd1680_start_write(void);
void ssd1680_write_byte(uint8_t data);
void ssd1680_end_write(void);
void ssd1680_write_byte_at(uint8_t x_pixel, uint8_t y, uint8_t data);
void ssd1680_refresh(void);
void ssd1680_sleep(void);
void send_command(uint8_t);
void send_data(uint8_t);
void ssd1680_set_data_entry_mode(uint8_t mode);
void ssd1680_draw_block8x8(SSD1680 *, uint8_t, uint8_t, const uint8_t *, uint8_t);
void ssd1680_draw_string(SSD1680 *, uint8_t, uint8_t, const uint8_t *, uint8_t (*)(char), uint8_t, uint8_t, uint8_t, uint8_t, uint8_t,  const char *);


#endif

