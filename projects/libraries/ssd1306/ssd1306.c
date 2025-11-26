#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <util/delay.h>
#include <i2c.h>
#include <font-transform.h>
#include <ssd1306.h>


static void ssd1306_write(SSD1306 *display, const uint8_t *data, uint8_t size) {
  i2c_start();
  i2c_write(display->address << 1); 
  for(uint8_t i = 0; i < size; i++) 
    i2c_write(data[i]);
  i2c_stop();
}

static void ssd1306_command(SSD1306 *display, uint8_t value) {
  uint8_t data[2];
  data[0] = SSD1306_COMMAND;
  data[1] = value;
  ssd1306_write(display, data, 2);
}

void ssd1306_init(SSD1306 *display, uint8_t address, uint8_t width, uint8_t height, uint8_t *buffer) {
  display->address   = address;
  display->width     = width;
  display->height    = height;
  display->pages     = height / 8;
  display->data_size = display->pages * display->width;
  display->data      = buffer;
  
  _delay_ms(100);
  
  ssd1306_command(display, SSD1306_DISPLAY_OFF);
  ssd1306_command(display, SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO);
  ssd1306_command(display, 0x80);
  ssd1306_command(display, SSD1306_SET_MULTIPLEX_RATIO);
  ssd1306_command(display, display->height - 1);
  ssd1306_command(display, SSD1306_SET_DISPLAY_OFFSET);
  ssd1306_command(display, 0x00);
  ssd1306_command(display, SSD1306_SET_START_LINE | 0x00);
  ssd1306_command(display, SSD1306_CHARGE_PUMP);
  ssd1306_command(display, 0x14);
  ssd1306_command(display, SSD1306_MEMORY_ADDR_MODE);
  ssd1306_command(display, 0x00);  
  ssd1306_command(display, SSD1306_SET_SEGMENT_REMAP | 0x01);
  ssd1306_command(display, SSD1306_COM_SCAN_DIR_DEC);
  
  if (display->height == 64) {
    ssd1306_command(display, SSD1306_SET_COM_PINS);
    ssd1306_command(display, 0x12);
    ssd1306_command(display, SSD1306_SET_CONTRAST_CONTROL);
    ssd1306_command(display, 0xCF);
  } else {
    ssd1306_command(display, SSD1306_SET_COM_PINS);
    ssd1306_command(display, 0x02);
    ssd1306_command(display, SSD1306_SET_CONTRAST_CONTROL);
    ssd1306_command(display, 0x8F);
  }
  
  ssd1306_command(display, SSD1306_SET_PRECHARGE_PERIOD);
  ssd1306_command(display, 0xF1);
  ssd1306_command(display, SSD1306_SET_VCOM_DESELECT);
  ssd1306_command(display, 0x40);
  ssd1306_command(display, SSD1306_DISPLAY_ALL_ON_RESUME);
  ssd1306_command(display, SSD1306_NORMAL_DISPLAY);
  ssd1306_command(display, SSD1306_DEACTIVATE_SCROLL);
  ssd1306_command(display, SSD1306_DISPLAY_ON);
  
  _delay_ms(100);
}


void ssd1306_control(SSD1306 *display, int op, uint8_t value) {
  switch(op) {
  case SSD1306_ON:
    ssd1306_command(display, SSD1306_DISPLAY_ON);
    break;
  case SSD1306_OFF:
    ssd1306_command(display, SSD1306_DISPLAY_OFF);
    break;
  case SSD1360_CONTRAST:
    ssd1306_command(display, SSD1306_SET_CONTRAST_CONTROL);
    ssd1306_command(display, value);
    break;
  case SSD1360_INVERT:
    ssd1306_command(display, value == 0 ? SSD1306_NORMAL_DISPLAY : SSD1306_INVERT_DISPLAY);
    break;
  }
}

void ssd1306_erase(SSD1306 *display, uint8_t color) {
  memset(display->data, color ? 0xFF : 0x00, display->data_size);
}

void ssd1306_set_pixel(SSD1306 *display, uint8_t x, uint8_t y, uint8_t value) {
  uint16_t offset;
  uint8_t  bit;

  if(x >= display->width || y >= display->height)
    return;
    
  offset = display->width * (y / 8) + x;
  bit = 1 << (y & 0x07);
  
  if (value == 1)
    display->data[offset] |=  bit;
  else
    display->data[offset] &= ~bit;
}

void ssd1306_draw_line(SSD1306 *display, int8_t x1, int8_t y1, int8_t x2, int8_t y2) {
  int8_t px, py;
  
  if(x1 == x2) {
    if (y1 > y2) 
      for(py = y2; py <= y1; py++)
        ssd1306_set_pixel(display, x1, py, PIXEL_ON);
    else
      for(py = y1; py <= y2; py++)
        ssd1306_set_pixel(display, x1, py, PIXEL_ON);
    return;
  }
  
  if (x1 > x2)
    for(px = x2; px <= x1; px++) 
      ssd1306_set_pixel(display, px, (uint8_t)((float)(y1-y2) / (float)(x1-x2) * (float)(px - x2) + (float)y2), PIXEL_ON);
  else
    for(px = x1; px <= x2; px++) 
      ssd1306_set_pixel(display, px, (uint8_t)((float)(y2-y1) / (float)(x2-x1) * (float)(px-x1) + (float)y1), PIXEL_ON);
}

void ssd1306_draw_filled_rectangle(SSD1306 *display, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t state) {
  uint8_t dx, dy;
  
  for(dx = 0; dx < width; ++dx)
    for(dy = 0; dy < height; ++dy)
      ssd1306_set_pixel(display, x + dx, y + dy, state);
}

void ssd1306_draw_rectangle(SSD1306 *display, uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
  ssd1306_draw_line(display, x,       y,        x+width, y);
  ssd1306_draw_line(display, x,       y+height, x+width, y+height);
  ssd1306_draw_line(display, x,       y,        x,       y+height);
  ssd1306_draw_line(display, x+width, y,        x+width, y+height);
}

void ssd1306_draw_block8x8(SSD1306 *display, uint8_t x, uint8_t y, const uint8_t *block) {
    for (uint8_t row = 0; row < 8; row++) {
        uint8_t byte = block[row];
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (byte & (1 << (7 - bit))) {
                ssd1306_set_pixel(display, x + bit, y + row, PIXEL_ON);
            }
        }
    }
}

void ssd1306_draw_string(SSD1306 *display, uint8_t x, uint8_t y, 
                         const uint8_t *font, 
                         uint8_t (*ascii_to_index)(char),
                         uint8_t double_width, uint8_t double_height, 
                         uint8_t rotation, uint8_t invert,
                         const char *s) {
    TransformedChar tc;
    uint8_t char_width = double_width ? 16 : 8;
    uint8_t char_height = double_height ? 16 : 8;
    int16_t cursor_x = x;
    int16_t cursor_y = y;
    
    while (*s) {
        // Convert ASCII to font index
        uint8_t index = ascii_to_index ? ascii_to_index(*s) : (uint8_t)*s;
        
        // Transform character
        transform_char(font, index, double_width, double_height, rotation, invert, &tc);
        
        // Draw character
        draw_transformed_char(display, cursor_x, cursor_y, &tc, 
                             (void (*)(void*, uint8_t, uint8_t, const uint8_t*))ssd1306_draw_block8x8);
        
        // Advance cursor based on rotation
        switch(rotation) {
            case ROTATE_0:   // Horizontal, left to right
                cursor_x += char_width;
                break;
            case ROTATE_90:  // Vertical, top to bottom
                cursor_y += char_height;
                break;
            case ROTATE_180: // Horizontal, right to left
                cursor_x -= char_width;
                break;
            case ROTATE_270: // Vertical, bottom to top
                cursor_y -= char_height;
                break;
        }
        
        s++;
    }
}

void ssd1306_refresh(SSD1306 *display) {
  uint16_t i;
  
  ssd1306_command(display, SSD1306_SET_COLUMN_ADDR);
  ssd1306_command(display, 0);   
  ssd1306_command(display, display->width - 1); 
  ssd1306_command(display, SSD1306_SET_PAGE_ADDR);
  ssd1306_command(display, 0);
  
  if (display->height == 64) 
    ssd1306_command(display, 7);
  else
    ssd1306_command(display, 3);
  
  // Send all data in ONE transaction (much faster!)
  i2c_start();
  i2c_write(display->address << 1);
  i2c_write(SSD1306_DATA_CONTINUE);  // 0x40 - data mode
  
  for (i = 0; i < display->data_size; i++) {
    i2c_write(display->data[i]);
  }
  
  i2c_stop();
}
