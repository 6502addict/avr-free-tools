#include <stdint.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "font-transform.h"

// PETSCII to screen code conversion
uint8_t petscii_to_screen(char c) {
    int16_t code = (int16_t)c;
    if (code >= 0 && code <= 31) return code + 128;
    else if (code >= 32 && code <= 63) return code;
    else if (code >= 64 && code <= 95) return code - 64;
    else if (code >= 96 && code <= 127) return code - 32;
    else if (code >= 128 && code <= 159) return code + 64;
    else if (code >= 160 && code <= 191) return code - 64;
    else if (code >= 192 && code <= 223) return code - 128;
    else if (code >= 224 && code <= 254) return code - 128;
    else if (code == 255) return 94;
    return 0;
}

// Get bit from 8x8 bitmap
static uint8_t get_bit(const uint8_t *bitmap, uint8_t x, uint8_t y) {
    return (bitmap[y] & (1 << (7 - x))) ? 1 : 0;
}

// Set bit in 8x8 block
static void set_bit(uint8_t *block, uint8_t x, uint8_t y) {
    block[y] |= (1 << (7 - x));
}

// Rotate 8x8 bitmap into 8x8 block
static void rotate_block(const uint8_t *src, uint8_t *dst, uint8_t rotation) {
    uint8_t x, y;
    
    memset(dst, 0, 8);
    
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            if (get_bit(src, x, y)) {
                uint8_t new_x, new_y;
                
                switch(rotation) {
                    case ROTATE_0:   // No rotation
                        new_x = x;
                        new_y = y;
                        break;
                    case ROTATE_90:  // 90° clockwise
                        new_x = 7 - y;
                        new_y = x;
                        break;
                    case ROTATE_180: // 180°
                        new_x = 7 - x;
                        new_y = 7 - y;
                        break;
                    case ROTATE_270: // 270° clockwise (90° left)
                        new_x = y;
                        new_y = 7 - x;
                        break;
                    default:
                        new_x = x;
                        new_y = y;
                        break;
                }
                
                set_bit(dst, new_x, new_y);
            }
        }
    }
}

void transform_char(const uint8_t *font8x8, char c, 
                    uint8_t double_width, uint8_t double_height, 
                    uint8_t rotation, uint8_t invert, TransformedChar *result) {
  //    uint8_t screen_code = petscii_to_screen(c);
  uint8_t code = (uint8_t)  c;
  uint8_t char_invert = (code & 0x80) ? 1 : 0;  // Bit 7 = per-char invert
  uint8_t index = code & 0x7F;                   // Bits 0-6 = font index
  uint8_t base[8];
  uint8_t rotated[8];
  
  // Clear result
  memset(result, 0, sizeof(TransformedChar));
  
  // Step 1: Get base 8x8 character from PROGMEM
  for (uint8_t i = 0; i < 8; i++) {
    base[i] = pgm_read_byte(&font8x8[index * 8 + i]);
  }

  // Step 2: Apply invert (global XOR per-char)
  uint8_t final_invert = invert ^ char_invert;  // XOR both flags
  if (final_invert) {
    for (uint8_t i = 0; i < 8; i++) {
      base[i] = ~base[i];
    }
  }
  // Step 3: Apply rotation
  rotate_block(base, rotated, rotation);
  
  // Step 4: Generate blocks based on scaling
  if (!double_width && !double_height) {
    // 8x8: Single block
    result->blocks_x = 1;
    result->blocks_y = 1;
    memcpy(result->block[0], rotated, 8);
    
  } else if (double_width && !double_height) {
    // 16x8: Two blocks horizontally
    result->blocks_x = 2;
    result->blocks_y = 1;
    
    // Clear blocks first
    memset(result->block[0], 0, 8);
    memset(result->block[1], 0, 8);
    
    for (uint8_t y = 0; y < 8; y++) {
      uint8_t row = rotated[y];
      
      // Build left block (pixels 0-3 doubled to 0-7)
      for (uint8_t x = 0; x < 4; x++) {
	if (row & (1 << (7 - x))) {
	  result->block[0][y] |= (1 << (7 - (x * 2)));
	  result->block[0][y] |= (1 << (7 - (x * 2 + 1)));
	}
      }
      
      // Build right block (pixels 4-7 doubled to 0-7)
      for (uint8_t x = 4; x < 8; x++) {
	if (row & (1 << (7 - x))) {
	  uint8_t local_x = x - 4;
	  result->block[1][y] |= (1 << (7 - (local_x * 2)));
	  result->block[1][y] |= (1 << (7 - (local_x * 2 + 1)));
	}
      }
    }
    
  } else if (!double_width && double_height) {
    // 8x16: Two blocks vertically
    result->blocks_x = 1;
    result->blocks_y = 2;
    
    // Clear blocks first
    memset(result->block[0], 0, 8);
    memset(result->block[2], 0, 8);
    
    // Top block: rows 0-3 doubled to rows 0-7
    for (uint8_t y = 0; y < 4; y++) {
      result->block[0][y * 2] = rotated[y];
      result->block[0][y * 2 + 1] = rotated[y];
    }
    
    // Bottom block: rows 4-7 doubled to rows 0-7
    for (uint8_t y = 4; y < 8; y++) {
      uint8_t local_y = y - 4;
      result->block[2][local_y * 2] = rotated[y];
      result->block[2][local_y * 2 + 1] = rotated[y];
    }
  } else {
    // 16x16: Four blocks (2x2)
    result->blocks_x = 2;
    result->blocks_y = 2;
    
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t x = 0; x < 8; x++) {
	if (get_bit(rotated, x, y)) {
	  // Double both dimensions
	  uint8_t block_x = (x < 4) ? 0 : 1;
	  uint8_t block_y = (y < 4) ? 0 : 2;
	  uint8_t block_idx = block_y + block_x;
	  uint8_t local_x = (x % 4) * 2;
	  uint8_t local_y = (y % 4) * 2;
	  
	  set_bit(result->block[block_idx], local_x, local_y);
	  set_bit(result->block[block_idx], local_x + 1, local_y);
	  set_bit(result->block[block_idx], local_x, local_y + 1);
	  set_bit(result->block[block_idx], local_x + 1, local_y + 1);
	}
      }
    }
  }
}

void draw_transformed_char(void *display, uint8_t x, uint8_t y,
			   const TransformedChar *tc,
			   void (*draw_block8x8)(void*, uint8_t, uint8_t, const uint8_t*)) {
  uint8_t bx, by;
  
  for (by = 0; by < tc->blocks_y; by++) {
    for (bx = 0; bx < tc->blocks_x; bx++) {
      uint8_t block_idx = by * 2 + bx;
      draw_block8x8(display, x + bx * 8, y + by * 8, tc->block[block_idx]);
    }
  }
}
