#ifndef PETSCII_CHARS_H
#define PETSCII_CHARS_H

// PETSCII Graphics Characters (screen codes)
// Use with petscii_to_screen() or directly if already converted

// Basic blocks
#define CHAR_SPACE          32
#define CHAR_FULL_BLOCK     160  // █ Full solid block
#define CHAR_HALF_TOP       223  // ▀ Upper half block
#define CHAR_HALF_BOTTOM    95   // ▄ Lower half block
#define CHAR_HALF_LEFT      161  // ▌ Left half block
#define CHAR_HALF_RIGHT     162  // ▐ Right half block

// Quarter blocks (useful for smooth bars)
#define CHAR_QUARTER_TL     108  // ▘ Top-left quarter
#define CHAR_QUARTER_TR     123  // ▝ Top-right quarter
#define CHAR_QUARTER_BL     124  // ▖ Bottom-left quarter
#define CHAR_QUARTER_BR     126  // ▗ Bottom-right quarter

// Lines and borders
#define CHAR_HLINE          64   // ─ Horizontal line
#define CHAR_VLINE          93   // │ Vertical line
#define CHAR_CORNER_TL      85   // ┌ Top-left corner
#define CHAR_CORNER_TR      73   // ┐ Top-right corner
#define CHAR_CORNER_BL      74   // └ Bottom-left corner
#define CHAR_CORNER_BR      75   // ┘ Bottom-right corner

// Card suits
#define CHAR_HEART          83   // ♥
#define CHAR_SPADE          65   // ♠ (pique)
#define CHAR_CLUB           88   // ♣
#define CHAR_DIAMOND        90   // ♦

// Useful symbols
#define CHAR_CIRCLE         81   // ●
#define CHAR_ARROW_UP       94   // ↑
#define CHAR_ARROW_DOWN     95   // ↓
#define CHAR_ARROW_LEFT     95   // ←
#define CHAR_ARROW_RIGHT    93   // →

#endif
