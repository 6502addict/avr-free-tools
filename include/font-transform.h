#ifndef FONT_TRANSFORM_H
#define FONT_TRANSFORM_H

// Rotation values
#define ROTATE_0    0  // No rotation
#define ROTATE_90   1  // 90° clockwise (right)
#define ROTATE_180  2  // 180° (upside down)
#define ROTATE_270  3  // 270° clockwise (90° left)

// Transformed character as array of 8x8 blocks
typedef struct {
  uint8_t blocks_x;      // Number of blocks horizontally (1 or 2)
  uint8_t blocks_y;      // Number of blocks vertically (1 or 2)
  uint8_t block[4][8];   // Up to 4 blocks of 8x8 (8 bytes each)
} TransformedChar;

// PETSCII to screen code conversion
uint8_t petscii_to_screen(char c);

// Transform character into 1-4 blocks of 8x8
// double_width: 0=normal, 1=double width
// double_height: 0=normal, 1=double height
// rotation: 0=0°, 1=90°, 2=180°, 3=270°
void transform_char(const uint8_t *font8x8, char c, 
                    uint8_t double_width, uint8_t double_height, 
                    uint8_t rotation, uint8_t invert,  
                    TransformedChar *result);

// Draw transformed char using 8x8 block callback
// draw_block8x8: callback(display, x, y, block_data[8])
void draw_transformed_char(void *display, uint8_t x, uint8_t y,
                          const TransformedChar *tc,
                          void (*draw_block8x8)(void*, uint8_t, uint8_t, const uint8_t*));

#endif
