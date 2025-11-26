#ifndef SSD1306_H_INCLUDED
#define SSD1306_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

// Inline hint for AVR GCC
#define HOTSPOT static inline __attribute__((always_inline))

#define SSD1306_COMMAND            0x00
#define SSD1306_DATA               0xC0
#define SSD1306_DATA_CONTINUE      0x40
#define SSD1306_ADDR               0x3C

#define SSD1306_ON        1
#define SSD1306_OFF       2
#define SSD1360_CONTRAST  3
#define SSD1360_INVERT    4

#define PIXEL_OFF        0
#define PIXEL_ON         1

#define ERASE_BLACK      0
#define ERASE_WHITE      1

// Command definitions
#define SSD1306_SET_CONTRAST_CONTROL                  0x81
#define SSD1306_DISPLAY_ALL_ON_RESUME                 0xA4
#define SSD1306_DISPLAY_ALL_ON                        0xA5
#define SSD1306_NORMAL_DISPLAY                        0xA6
#define SSD1306_INVERT_DISPLAY                        0xA7
#define SSD1306_DISPLAY_OFF                           0xAE
#define SSD1306_DISPLAY_ON                            0xAF
#define SSD1306_NOP                                   0xE3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL               0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL                0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL  0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL   0x2A
#define SSD1306_DEACTIVATE_SCROLL                     0x2E
#define SSD1306_ACTIVATE_SCROLL                       0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA              0xA3
#define SSD1306_SET_LOWER_COLUMN                      0x00
#define SSD1306_SET_HIGHER_COLUMN                     0x10
#define SSD1306_MEMORY_ADDR_MODE                      0x20
#define SSD1306_SET_COLUMN_ADDR                       0x21
#define SSD1306_SET_PAGE_ADDR                         0x22
#define SSD1306_SET_START_LINE                        0x40
#define SSD1306_SET_SEGMENT_REMAP                     0xA0
#define SSD1306_SET_MULTIPLEX_RATIO                   0xA8
#define SSD1306_COM_SCAN_DIR_INC                      0xC0
#define SSD1306_COM_SCAN_DIR_DEC                      0xC8
#define SSD1306_SET_DISPLAY_OFFSET                    0xD3
#define SSD1306_SET_COM_PINS                          0xDA
#define SSD1306_CHARGE_PUMP                           0x8D
#define SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO           0xD5
#define SSD1306_SET_PRECHARGE_PERIOD                  0xD9
#define SSD1306_SET_VCOM_DESELECT                     0xDB

typedef struct s_ssd1306 {
  uint8_t     address; 
  uint8_t     width; 
  uint8_t     height;
  uint8_t     pages;
  uint16_t    data_size;
  uint8_t    *data;
} SSD1306;

void ssd1306_init(SSD1306 *, uint8_t, uint8_t, uint8_t, uint8_t *);
void ssd1306_control(SSD1306 *, int, uint8_t);
void ssd1306_refresh(SSD1306 *);
void ssd1306_erase(SSD1306 *, uint8_t);
void ssd1306_set_pixel(SSD1306 *, uint8_t, uint8_t, uint8_t);
void ssd1306_draw_line(SSD1306 *, int8_t, int8_t, int8_t, int8_t);
void ssd1306_draw_filled_rectangle(SSD1306 *, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void ssd1306_draw_rectangle(SSD1306 *, uint8_t , uint8_t , uint8_t, uint8_t);
void ssd1306_draw_block8x8(SSD1306 *, uint8_t, uint8_t, const uint8_t *);
void ssd1306_draw_string(SSD1306 *, uint8_t, uint8_t, const uint8_t *, uint8_t (*)(char), uint8_t, uint8_t, uint8_t, uint8_t, const char *);

#endif
