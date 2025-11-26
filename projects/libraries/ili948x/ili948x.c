#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <ili948x.h>
#include <font-transform.h>

// Pin control macros
#define LCD_RD_LOW()    (LCD_CTRL_PORT &= ~(1 << LCD_RD_PIN))
#define LCD_RD_HIGH()   (LCD_CTRL_PORT |= (1 << LCD_RD_PIN))
#define LCD_WR_LOW()    (LCD_CTRL_PORT &= ~(1 << LCD_WR_PIN))
#define LCD_WR_HIGH()   (LCD_CTRL_PORT |= (1 << LCD_WR_PIN))
#define LCD_RS_LOW()    (LCD_CTRL_PORT &= ~(1 << LCD_RS_PIN))
#define LCD_RS_HIGH()   (LCD_CTRL_PORT |= (1 << LCD_RS_PIN))
#define LCD_CS_LOW()    (LCD_CTRL_PORT &= ~(1 << LCD_CS_PIN))
#define LCD_CS_HIGH()   (LCD_CTRL_PORT |= (1 << LCD_CS_PIN))
#define LCD_RST_LOW()   (LCD_CTRL_PORT &= ~(1 << LCD_RST_PIN))
#define LCD_RST_HIGH()  (LCD_CTRL_PORT |= (1 << LCD_RST_PIN))

// 5x7 Font data
/*
static const uint8_t font_5x7[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, // 0x20 ' '
    0x00, 0x00, 0x5F, 0x00, 0x00, // 0x21 '!'
    0x00, 0x07, 0x00, 0x07, 0x00, // 0x22 '"'
    0x14, 0x7F, 0x14, 0x7F, 0x14, // 0x23 '#'
    0x24, 0x2A, 0x7F, 0x2A, 0x12, // 0x24 '$'
    0x23, 0x13, 0x08, 0x64, 0x62, // 0x25 '%'
    0x36, 0x49, 0x55, 0x22, 0x50, // 0x26 '&'
    0x00, 0x05, 0x03, 0x00, 0x00, // 0x27 '''
    0x00, 0x1C, 0x22, 0x41, 0x00, // 0x28 '('
    0x00, 0x41, 0x22, 0x1C, 0x00, // 0x29 ')'
    0x14, 0x08, 0x3E, 0x08, 0x14, // 0x2A '*'
    0x08, 0x08, 0x3E, 0x08, 0x08, // 0x2B '+'
    0x00, 0x50, 0x30, 0x00, 0x00, // 0x2C ','
    0x08, 0x08, 0x08, 0x08, 0x08, // 0x2D '-'
    0x00, 0x60, 0x60, 0x00, 0x00, // 0x2E '.'
    0x20, 0x10, 0x08, 0x04, 0x02, // 0x2F '/'
    0x3E, 0x51, 0x49, 0x45, 0x3E, // 0x30 '0'
    0x00, 0x42, 0x7F, 0x40, 0x00, // 0x31 '1'
    0x42, 0x61, 0x51, 0x49, 0x46, // 0x32 '2'
    0x21, 0x41, 0x45, 0x4B, 0x31, // 0x33 '3'
    0x18, 0x14, 0x12, 0x7F, 0x10, // 0x34 '4'
    0x27, 0x45, 0x45, 0x45, 0x39, // 0x35 '5'
    0x3C, 0x4A, 0x49, 0x49, 0x30, // 0x36 '6'
    0x01, 0x71, 0x09, 0x05, 0x03, // 0x37 '7'
    0x36, 0x49, 0x49, 0x49, 0x36, // 0x38 '8'
    0x06, 0x49, 0x49, 0x29, 0x1E, // 0x39 '9'
    0x00, 0x36, 0x36, 0x00, 0x00, // 0x3A ':'
    0x00, 0x56, 0x36, 0x00, 0x00, // 0x3B ';'
    0x08, 0x14, 0x22, 0x41, 0x00, // 0x3C '<'
    0x14, 0x14, 0x14, 0x14, 0x14, // 0x3D '='
    0x00, 0x41, 0x22, 0x14, 0x08, // 0x3E '>'
    0x02, 0x01, 0x51, 0x09, 0x06, // 0x3F '?'
    0x32, 0x49, 0x79, 0x41, 0x3E, // 0x40 '@'
    0x7E, 0x11, 0x11, 0x11, 0x7E, // 0x41 'A'
    0x7F, 0x49, 0x49, 0x49, 0x36, // 0x42 'B'
    0x3E, 0x41, 0x41, 0x41, 0x22, // 0x43 'C'
    0x7F, 0x41, 0x41, 0x22, 0x1C, // 0x44 'D'
    0x7F, 0x49, 0x49, 0x49, 0x41, // 0x45 'E'
    0x7F, 0x09, 0x09, 0x09, 0x01, // 0x46 'F'
    0x3E, 0x41, 0x49, 0x49, 0x7A, // 0x47 'G'
    0x7F, 0x08, 0x08, 0x08, 0x7F, // 0x48 'H'
    0x00, 0x41, 0x7F, 0x41, 0x00, // 0x49 'I'
    0x20, 0x40, 0x41, 0x3F, 0x01, // 0x4A 'J'
    0x7F, 0x08, 0x14, 0x22, 0x41, // 0x4B 'K'
    0x7F, 0x40, 0x40, 0x40, 0x40, // 0x4C 'L'
    0x7F, 0x02, 0x0C, 0x02, 0x7F, // 0x4D 'M'
    0x7F, 0x04, 0x08, 0x10, 0x7F, // 0x4E 'N'
    0x3E, 0x41, 0x41, 0x41, 0x3E, // 0x4F 'O'
    0x7F, 0x09, 0x09, 0x09, 0x06, // 0x50 'P'
    0x3E, 0x41, 0x51, 0x21, 0x5E, // 0x51 'Q'
    0x7F, 0x09, 0x19, 0x29, 0x46, // 0x52 'R'
    0x46, 0x49, 0x49, 0x49, 0x31, // 0x53 'S'
    0x01, 0x01, 0x7F, 0x01, 0x01, // 0x54 'T'
    0x3F, 0x40, 0x40, 0x40, 0x3F, // 0x55 'U'
    0x1F, 0x20, 0x40, 0x20, 0x1F, // 0x56 'V'
    0x3F, 0x40, 0x38, 0x40, 0x3F, // 0x57 'W'
    0x63, 0x14, 0x08, 0x14, 0x63, // 0x58 'X'
    0x07, 0x08, 0x70, 0x08, 0x07, // 0x59 'Y'
    0x61, 0x51, 0x49, 0x45, 0x43, // 0x5A 'Z'
    0x00, 0x7F, 0x41, 0x41, 0x00, // 0x5B '['
    0x02, 0x04, 0x08, 0x10, 0x20, // 0x5C '\'
    0x00, 0x41, 0x41, 0x7F, 0x00, // 0x5D ']'
    0x04, 0x02, 0x01, 0x02, 0x04, // 0x5E '^'
    0x40, 0x40, 0x40, 0x40, 0x40, // 0x5F '_'
};
*/

static void lcd_set_data_dir_out(void) {
    DDRB |= PORTB_DATA_MASK;
    DDRD |= PORTD_DATA_MASK;
}

static void lcd_set_data_dir_in(void) {
    DDRB &= ~PORTB_DATA_MASK;
    DDRD &= ~PORTD_DATA_MASK;
}

static void lcd_write_8(uint8_t data) {
    uint8_t portb_data = (data & 0x03);
    uint8_t portd_data = (data & 0xFC);
    PORTB = (PORTB & ~PORTB_DATA_MASK) | portb_data;
    PORTD = (PORTD & ~PORTD_DATA_MASK) | portd_data;
}

static uint8_t lcd_read_8(void) {
    return (PINB & PORTB_DATA_MASK) | (PIND & PORTD_DATA_MASK);
}

void ili948x_write_command(uint8_t cmd) {
    LCD_RS_LOW();
    LCD_CS_LOW();
    LCD_RD_HIGH();
    LCD_WR_LOW();
    lcd_write_8(cmd);
    LCD_WR_HIGH();
    LCD_CS_HIGH();
}

void ili948x_write_data(uint8_t data) {
    LCD_RS_HIGH();
    LCD_CS_LOW();
    LCD_RD_HIGH();
    LCD_WR_LOW();
    lcd_write_8(data);
    LCD_WR_HIGH();
    LCD_CS_HIGH();
}

void ili948x_write_data_16(uint16_t data) {
    LCD_RS_HIGH();
    LCD_CS_LOW();
    LCD_RD_HIGH();
    
    LCD_WR_LOW();
    lcd_write_8(data >> 8);
    LCD_WR_HIGH();
    
    LCD_WR_LOW();
    lcd_write_8(data & 0xFF);
    LCD_WR_HIGH();
    
    LCD_CS_HIGH();
}

static uint8_t lcd_read_data(void) {
    uint8_t data;
    lcd_set_data_dir_in();
    LCD_RS_HIGH();
    LCD_CS_LOW();
    LCD_WR_HIGH();
    LCD_RD_LOW();
    _delay_us(1);
    data = lcd_read_8();
    LCD_RD_HIGH();
    LCD_CS_HIGH();
    lcd_set_data_dir_out();
    return data;
}

uint16_t ili948x_read_id(void) {
    uint16_t id;
    ili948x_write_command(0xD3);
    lcd_read_data();
    lcd_read_data();
    id = (lcd_read_data() << 8);
    id |= lcd_read_data();
    return id;
}

void ili948x_set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    ili948x_write_command(0x2A);
    ili948x_write_data(x1 >> 8);
    ili948x_write_data(x1 & 0xFF);
    ili948x_write_data(x2 >> 8);
    ili948x_write_data(x2 & 0xFF);

    ili948x_write_command(0x2B);
    ili948x_write_data(y1 >> 8);
    ili948x_write_data(y1 & 0xFF);
    ili948x_write_data(y2 >> 8);
    ili948x_write_data(y2 & 0xFF);

    ili948x_write_command(0x2C);
}

void ili948x_init(ILI948X *display) {
    display->width = ILI948X_WIDTH;
    display->height = ILI948X_HEIGHT;
    
    // Set control pins as output
    LCD_CTRL_DDR |= (1 << LCD_RD_PIN) | (1 << LCD_WR_PIN) | (1 << LCD_RS_PIN) |
                    (1 << LCD_CS_PIN) | (1 << LCD_RST_PIN);
    
    lcd_set_data_dir_out();
    
    LCD_RD_HIGH();
    LCD_WR_HIGH();
    LCD_RS_HIGH();
    LCD_CS_HIGH();
    
    // Hardware Reset
    LCD_RST_HIGH();
    _delay_ms(10);
    LCD_RST_LOW();
    _delay_ms(50);
    LCD_RST_HIGH();
    _delay_ms(120);
    
    // ILI9488 Init sequence (keeping your working code)
    ili948x_write_command(0xE0);
    ili948x_write_data(0x00); ili948x_write_data(0x03); ili948x_write_data(0x09);
    ili948x_write_data(0x08); ili948x_write_data(0x16); ili948x_write_data(0x0A);
    ili948x_write_data(0x3F); ili948x_write_data(0x78); ili948x_write_data(0x4C);
    ili948x_write_data(0x09); ili948x_write_data(0x0A); ili948x_write_data(0x08);
    ili948x_write_data(0x16); ili948x_write_data(0x1A); ili948x_write_data(0x0F);

    ili948x_write_command(0xE1);
    ili948x_write_data(0x00); ili948x_write_data(0x16); ili948x_write_data(0x19);
    ili948x_write_data(0x03); ili948x_write_data(0x0F); ili948x_write_data(0x05);
    ili948x_write_data(0x32); ili948x_write_data(0x45); ili948x_write_data(0x46);
    ili948x_write_data(0x04); ili948x_write_data(0x0E); ili948x_write_data(0x0D);
    ili948x_write_data(0x35); ili948x_write_data(0x37); ili948x_write_data(0x0F);

    ili948x_write_command(0xC0);
    ili948x_write_data(0x17); ili948x_write_data(0x15);

    ili948x_write_command(0xC1);
    ili948x_write_data(0x41);

    ili948x_write_command(0xC5);
    ili948x_write_data(0x00); ili948x_write_data(0x12); ili948x_write_data(0x80);

    ili948x_write_command(0x36);
    ili948x_write_data(0x38);

    ili948x_write_command(0x3A);
    ili948x_write_data(0x55);

    ili948x_write_command(0xB0);
    ili948x_write_data(0x00);

    ili948x_write_command(0xB1);
    ili948x_write_data(0xA0);

    ili948x_write_command(0xB4);
    ili948x_write_data(0x02);

    ili948x_write_command(0xB6);
    ili948x_write_data(0x02); ili948x_write_data(0x02);

    ili948x_write_command(0xE9);
    ili948x_write_data(0x00);

    ili948x_write_command(0xF7);
    ili948x_write_data(0xA9); ili948x_write_data(0x51);
    ili948x_write_data(0x2C); ili948x_write_data(0x82);

    ili948x_write_command(0x11);
    _delay_ms(120);

    ili948x_write_command(0x29);
    _delay_ms(25);
}

void ili948x_fill_screen(ILI948X *display, uint16_t color) {
    ili948x_set_window(0, 0, display->width - 1, display->height - 1);
    
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    
    LCD_RS_HIGH();
    LCD_CS_LOW();
    LCD_RD_HIGH();
    
    for (long i = 0; i < (long)display->width * display->height; i++) {
        LCD_WR_LOW();
        lcd_write_8(hi);
        LCD_WR_HIGH();
        LCD_WR_LOW();
        lcd_write_8(lo);
        LCD_WR_HIGH();
    }
    
    LCD_CS_HIGH();
}

/*
void ili948x_draw_char(ILI948X *display, uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg) {
    (void)display;
    
    if (c < 0x20 || c > 0x5F) c = 0x20;
    
    ili948x_set_window(x, y, x + 4, y + 7);
    
    LCD_RS_HIGH();
    LCD_CS_LOW();
    LCD_RD_HIGH();
    
    for (uint8_t row = 0; row < 8; row++) {
        for (uint8_t col = 0; col < 5; col++) {
            uint8_t line = pgm_read_byte(&font_5x7[(c - 0x20) * 5 + col]);
            uint16_t pixel_color = (line & (1 << row)) ? color : bg;
            
            LCD_WR_LOW();
            lcd_write_8(pixel_color >> 8);
            LCD_WR_HIGH();
            LCD_WR_LOW();
            lcd_write_8(pixel_color & 0xFF);
            LCD_WR_HIGH();
        }
    }
    
    LCD_CS_HIGH();
}
*/

/*
void ili948x_draw_string(ILI948X *display, uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg) {
    while (*str) {
        ili948x_draw_char(display, x, y, *str++, color, bg);
        x += 6;
        
        if (x > (display->width - 6)) {
            y += 8;
            x = 0;
        }
    }
}
*/

void ili948x_draw_block8x8(ILI948X *display, uint8_t x, uint8_t y, const uint8_t *block, uint16_t color, uint16_t bg) {
  (void)display;
  
  // Set window for 8x8 block
  ili948x_set_window(x, y, x + 7, y + 7);
  
  // Prepare for burst write
  LCD_RS_HIGH();
  LCD_CS_LOW();
  LCD_RD_HIGH();
  
  // Draw all 64 pixels
  for (uint8_t row = 0; row < 8; row++) {
    uint8_t byte = block[row];
    
    for (uint8_t col = 0; col < 8; col++) {
      uint16_t pixel_color = (byte & (1 << (7 - col))) ? color : bg;
      
      // Write 16-bit color
      LCD_WR_LOW();
      lcd_write_8(pixel_color >> 8);
      LCD_WR_HIGH();
      LCD_WR_LOW();
      lcd_write_8(pixel_color & 0xFF);
      LCD_WR_HIGH();
    }
  }
  
  LCD_CS_HIGH();
}

void ili948x_draw_string_8x8(ILI948X *display, uint16_t x, uint16_t y,
			     const uint8_t *font,
			     uint8_t (*ascii_to_index)(char),
			     uint8_t double_width, uint8_t double_height,
			     uint8_t rotation, uint8_t invert,
			     uint16_t color, uint16_t bg,
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
    
    // Draw each block
    for (uint8_t by = 0; by < tc.blocks_y; by++) {
      for (uint8_t bx = 0; bx < tc.blocks_x; bx++) {
	uint8_t block_idx = by * 2 + bx;
	ili948x_draw_block8x8(display, cursor_x + bx * 8, cursor_y + by * 8, 
			      tc.block[block_idx], color, bg);
      }
    }
    
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
