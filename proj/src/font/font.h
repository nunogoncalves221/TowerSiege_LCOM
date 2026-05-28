#ifndef FONT_H
#define FONT_H

#include <stdint.h>

// Draw a string at (x, y) tinted to the given AARRGGBB color.
// Returns the total pixel width drawn.
uint16_t font_draw_string(uint16_t x, uint16_t y, const char *s, uint32_t color);

// Return the pixel width a string would occupy without drawing.
uint16_t font_string_width(const char *s);

#endif
