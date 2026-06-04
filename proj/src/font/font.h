/**
 * @file font.h
 * @brief Bitmap font rendering and string-width measurement.
 *
 * Provides functions to draw text strings and to measure their pixel width
 * before drawing. All glyphs are rendered from a built-in bitmap font;
 * characters are coloured by tinting each non-transparent pixel with the
 * supplied AARRGGBB colour.
 *
 * @note Only uppercase letters, digits, and spaces are supported by the
 *       built-in glyph set.
 */

#ifndef FONT_H
#define FONT_H

#include <stdint.h>

/** @brief Pixel width of a letter glyph at scale 1. */
#define FONT_LETTER_W 18

/** @brief Pixel width of a digit glyph at scale 1. */
#define FONT_DIGIT_W  17

/** @brief Pixel width of a space character at scale 1. */
#define FONT_SPACE_W  8

/** @brief Horizontal pixel gap inserted between consecutive glyphs. */
#define CHAR_SPACING  2

/**
 * @brief Draws a string at the given screen position.
 *
 * Each pixel of every glyph is tinted to @p color. Transparent glyph
 * pixels are skipped.
 *
 * @param x     Left edge of the first character, in screen pixels.
 * @param y     Top  edge of the first character, in screen pixels.
 * @param s     Null-terminated string to draw.
 * @param color Tint colour in AARRGGBB format.
 * @return Total pixel width rendered (same value as font_string_width()).
 */
uint16_t font_draw_string(uint16_t x, uint16_t y, const char *s, uint32_t color);

/**
 * @brief Returns the pixel width a string would occupy without drawing it.
 *
 * Useful for centring or right-aligning text before committing to a draw call.
 *
 * @param s Null-terminated string to measure.
 * @return Width in pixels.
 */
uint16_t font_string_width(const char *s);

/**
 * @brief Draws a string with each glyph pixel scaled to a square block.
 *
 * Every logical pixel of every glyph is rendered as a @p scale × @p scale
 * block of screen pixels, producing an integer-scaled appearance.
 *
 * @param x     Left edge of the first character, in screen pixels.
 * @param y     Top  edge of the first character, in screen pixels.
 * @param s     Null-terminated string to draw.
 * @param color Tint colour in AARRGGBB format.
 * @param scale Integer scale factor (1 = normal size, 2 = double, …).
 * @return Total pixel width rendered at the given scale.
 */
uint16_t font_draw_string_scaled(uint16_t x, uint16_t y, const char *s, uint32_t color, uint8_t scale);

/**
 * @brief Returns the pixel width a scaled string would occupy without drawing it.
 *
 * @param s     Null-terminated string to measure.
 * @param scale Integer scale factor (must match the intended draw call).
 * @return Width in pixels at the given scale.
 */
uint16_t font_string_width_scaled(const char *s, uint8_t scale);

#endif /* FONT_H */
