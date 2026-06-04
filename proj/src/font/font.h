#ifndef FONT_H
#define FONT_H

#include <stdint.h>

#define FONT_LETTER_W 18
#define FONT_DIGIT_W  17
#define FONT_SPACE_W  8
#define CHAR_SPACING  2

// Draw a string at (x, y) tinted to the given AARRGGBB color.
// Returns the total pixel width drawn.
uint16_t font_draw_string(uint16_t x, uint16_t y, const char *s, uint32_t color);

// Return the pixel width a string would occupy without drawing.
uint16_t font_string_width(const char *s);

// Same as above but each glyph pixel is drawn as a scale×scale block.
uint16_t font_draw_string_scaled(uint16_t x, uint16_t y, const char *s, uint32_t color, uint8_t scale);
uint16_t font_string_width_scaled(const char *s, uint8_t scale);

#endif
