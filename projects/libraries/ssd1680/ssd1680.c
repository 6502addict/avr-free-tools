#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <util/delay.h>
#include <i2c.h>
#include <font-transform.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <ssd1680.h>

// SSD1680 commands
#define CMD_DRIVER_OUTPUT_CONTROL          0x01
#define CMD_GATE_DRIVING_VOLTAGE           0x03
#define CMD_SOURCE_DRIVING_VOLTAGE         0x04
#define CMD_DEEP_SLEEP_MODE                0x10
#define CMD_DATA_ENTRY_MODE                0x11
#define CMD_SW_RESET                       0x12
#define CMD_MASTER_ACTIVATION              0x20
#define CMD_DISPLAY_UPDATE_CONTROL_2       0x22
#define CMD_WRITE_RAM_BW                   0x24
#define CMD_WRITE_RAM_RED                  0x26
#define CMD_WRITE_VCOM_REGISTER            0x2C
#define CMD_WRITE_LUT_REGISTER             0x32
#define CMD_SET_RAM_X_ADDRESS_COUNTER      0x4E
#define CMD_SET_RAM_Y_ADDRESS_COUNTER      0x4F
#define CMD_SET_RAM_X_ADDRESS_START_END    0x44
#define CMD_SET_RAM_Y_ADDRESS_START_END    0x45

// Waveshare 2.13" V4 LUT
static const uint8_t lut_full_update[] PROGMEM = {
    0x80, 0x4A, 0x40, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0x10, 0x4A, 0x80, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0x80, 0x4A, 0x40, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0x10, 0x4A, 0x80, 0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0xF,  0x0,  0x0,  0x0,  0x0,
    0xF,  0x0,  0x0,  0xF,  0x0,
    0xF,  0x0,  0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,
};

// Pin control macros
#define CS_LOW()   SSD1680_CS_PORT &= ~(1 << SSD1680_CS_PIN)
#define CS_HIGH()  SSD1680_CS_PORT |= (1 << SSD1680_CS_PIN)
#define DC_LOW()   SSD1680_DC_PORT &= ~(1 << SSD1680_DC_PIN)
#define DC_HIGH()  SSD1680_DC_PORT |= (1 << SSD1680_DC_PIN)
#define RST_LOW()  SSD1680_RST_PORT &= ~(1 << SSD1680_RST_PIN)
#define RST_HIGH() SSD1680_RST_PORT |= (1 << SSD1680_RST_PIN)
#define IS_BUSY()  (SSD1680_BUSY_PIN & (1 << SSD1680_BUSY_BIT))

// Initialize SPI
static void spi_init(void) {
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
  DDRB |= (1 << PB3) | (1 << PB5);
  SPCR = (1 << SPE) | (1 << MSTR);
  SPSR |= (1 << SPI2X);
#elif defined(__AVR_ATmega1284P__)
  DDRB |= (1 << PB5) | (1 << PB7);
  SPCR = (1 << SPE) | (1 << MSTR);
  SPSR |= (1 << SPI2X);
#elif defined(__AVR_ATmega2560__)
  DDRB |= (1 << PB2) | (1 << PB1);
  SPCR = (1 << SPE) | (1 << MSTR);
  SPSR |= (1 << SPI2X);
#endif
}

// Send byte via SPI
static void spi_send(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
}

// Wait for display not busy
static void wait_busy(void) {
  uint16_t timeout = 0;
  while (IS_BUSY()) {
    _delay_ms(10);
    timeout++;
    if (timeout > 500) break;  // 5 second timeout
  }
}

// Send command
void send_command(uint8_t cmd) {
  DC_LOW();
  CS_LOW();
  spi_send(cmd);
  CS_HIGH();
}

// Send data
void send_data(uint8_t data) {
  DC_HIGH();
  CS_LOW();
  spi_send(data);
  CS_HIGH();
}

// Hardware reset
static void reset(void) {
  RST_HIGH();
  _delay_ms(20);
  RST_LOW();
  _delay_ms(2);
  RST_HIGH();
  _delay_ms(20);
}

// Set memory pointer
static void set_memory_pointer(uint8_t x, uint8_t y) {
  send_command(CMD_SET_RAM_X_ADDRESS_COUNTER);
  send_data(x);
  
  send_command(CMD_SET_RAM_Y_ADDRESS_COUNTER);
  send_data(y);
  send_data(y >> 8);
}

void ssd1680_set_data_entry_mode(uint8_t mode) {
  send_command(CMD_DATA_ENTRY_MODE);
  send_data(mode);
}

// Initialize display - Waveshare V4 specific
void ssd1680_init(void) {
  // Setup pins
  SSD1680_CS_DDR |= (1 << SSD1680_CS_PIN);
  SSD1680_DC_DDR |= (1 << SSD1680_DC_PIN);
  SSD1680_RST_DDR |= (1 << SSD1680_RST_PIN);
  SSD1680_BUSY_DDR &= ~(1 << SSD1680_BUSY_BIT);
  
  CS_HIGH();
  
  spi_init();
  reset();
  wait_busy();
  
  // Software reset
  send_command(CMD_SW_RESET);
  wait_busy();
  
  // Driver output control - 250 lines
  send_command(CMD_DRIVER_OUTPUT_CONTROL);
  send_data((SSD1680_HEIGHT - 1) & 0xFF);        // 249 = 0xF9
  send_data(((SSD1680_HEIGHT - 1) >> 8) & 0xFF); // 0x00
  send_data(0x00);
  
  // Data entry mode: X increment, Y increment
  send_command(CMD_DATA_ENTRY_MODE);
  send_data(0x03);
  
  // Set RAM X address (0-15 for 128 pixels, only 122 used)
  send_command(CMD_SET_RAM_X_ADDRESS_START_END);
  send_data(0x00);
  send_data(0x0F);  // 15 = 0x0F
  
  // Set RAM Y address (0-249 for 250 lines)
  send_command(CMD_SET_RAM_Y_ADDRESS_START_END);
  send_data(0x00);
  send_data(0x00);
  send_data((SSD1680_HEIGHT - 1) & 0xFF);        // 249 = 0xF9
  send_data(((SSD1680_HEIGHT - 1) >> 8) & 0xFF); // 0x00
  
  // Border waveform
  send_command(0x3C);
  send_data(0x05);
  
  // Temperature sensor
  send_command(0x18);
  send_data(0x80);
  
  // Display update control
  send_command(0x22);
  send_data(0xB1);
  send_command(0x20);
  wait_busy();
  
  // Load LUT
  send_command(CMD_WRITE_LUT_REGISTER);
  for (uint8_t i = 0; i < sizeof(lut_full_update); i++) {
    send_data(pgm_read_byte(&lut_full_update[i]));
  }
}

// Clear display
void ssd1680_clear(void) {
  set_memory_pointer(0, 0);
  
  send_command(CMD_WRITE_RAM_BW);
  DC_HIGH();
  CS_LOW();
  
  for (uint16_t i = 0; i < SSD1680_BUFFER_SIZE; i++) {
    spi_send(0xFF);
  }
  
  CS_HIGH();
}

void ssd1680_fill_black(void) {
  set_memory_pointer(0, 0);
  send_command(CMD_WRITE_RAM_BW);
  DC_HIGH();
  CS_LOW();
  
  for (uint16_t i = 0; i < SSD1680_BUFFER_SIZE; i++) {
    spi_send(0x00);  // Black
  }
  CS_HIGH();
}

// Start write to BW layer
void ssd1680_start_write(void) {
  set_memory_pointer(0, 0);
  send_command(CMD_WRITE_RAM_BW);
  DC_HIGH();
  CS_LOW();
}

// Write byte
void ssd1680_write_byte(uint8_t data) {
  spi_send(data);
}

// End write
void ssd1680_end_write(void) {
  CS_HIGH();
}

// Write byte at specific position
void ssd1680_write_byte_at(uint8_t x_pixel, uint8_t y, uint8_t data) {
  uint8_t x_byte = x_pixel / 8;
  
  set_memory_pointer(x_byte, y);
  send_command(CMD_WRITE_RAM_BW);
  DC_HIGH();
  CS_LOW();
  spi_send(data);
  CS_HIGH();
}

// Refresh display
void ssd1680_refresh(void) {
  send_command(CMD_DISPLAY_UPDATE_CONTROL_2);
  send_data(0xF7);
  
  send_command(CMD_MASTER_ACTIVATION);
  wait_busy();
}

// Sleep mode
void ssd1680_sleep(void) {
  send_command(CMD_DEEP_SLEEP_MODE);
  send_data(0x01);
}

void ssd1680_draw_block8x8(SSD1680 *display, uint8_t x_pixel, uint8_t y_pixel, const uint8_t *block, uint8_t color) {
  (void)display;
  
  // x_pixel: pixel position (convert to byte)
  // y_pixel: pixel position
  uint8_t x_byte = x_pixel / 8;
  
  // Write 8 columns (rotated for display orientation)
  for (uint8_t col = 0; col < 8; col++) {
    uint8_t byte = 0;
    
    // Extract bit 'col' from each of the 8 rows
    for (uint8_t row = 0; row < 8; row++) {
      if (block[row] & (1 << (7 - col))) {
	byte |= (1 << row);
      }
    }
    
    // Invert if black on white background
    if (color == COLOR_BLACK) {
      byte = ~byte;
    }
    
    // Write byte at position
    ssd1680_write_byte_at(x_byte * 8, y_pixel + col, byte);
  }
}


void ssd1680_draw_string(SSD1680 *display, uint8_t row, uint8_t col,
                         const uint8_t *font,
                         uint8_t (*ascii_to_index)(char),
                         uint8_t double_width, uint8_t double_height,
                         uint8_t rotation, uint8_t invert,
                         uint8_t color,
                         const char *s) {
  TransformedChar tc;
  uint8_t char_height = double_height ? 16 : 8;
  int16_t cursor_x = col;  // Pixel position (0-249)
  
  (void)display;
  (void)rotation;  // For SSD1680, always rotated 90° internally
  
  while (*s) {
    // Convert ASCII to font index
    uint8_t index = ascii_to_index ? ascii_to_index(*s) : (uint8_t)*s;
    
    // Transform character (always ROTATE_90 for this display)
    transform_char(font, index, double_width, double_height, ROTATE_0, invert, &tc);
    
    // Draw using inverted row coordinate like my_draw_string
    uint8_t x_byte = 15 - row;
    
    // Draw each block
    for (uint8_t by = 0; by < tc.blocks_y; by++) {
      for (uint8_t bx = 0; bx < tc.blocks_x; bx++) {
	uint8_t block_idx = by * 2 + bx;
        
	// Write each column of the block
	for (uint8_t col_offset = 0; col_offset < 8; col_offset++) {
	  uint8_t byte = 0;
          
	  // Extract bits for this column
	  for (uint8_t row_bit = 0; row_bit < 8; row_bit++) {
	    if (tc.block[block_idx][row_bit] & (1 << (7 - col_offset))) {
	      byte |= (1 << row_bit);
	    }
	  }
          
	  if (color == COLOR_BLACK) {
	    byte = ~byte;
	  }
          
	  ssd1680_write_byte_at((x_byte + bx) * 8, cursor_x + by * 8 + col_offset, byte);
	}
      }
    }
    
    // Move cursor
    cursor_x += char_height;
    s++;
  }
}
