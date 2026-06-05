/**
 * @file nameentry.h
 * @brief Single-line name input widget for the high-score entry screen.
 *
 * Manages a fixed-length text buffer that the player edits with the keyboard.
 * The caller drives it by forwarding raw scancodes and queries the result
 * once the player confirms with Enter.
 */

#ifndef NAMEENTRY_H
#define NAMEENTRY_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Clears the name buffer and resets cursor position.
 *
 * Must be called before showing the name-entry screen.
 */
void nameentry_reset(void);

/**
 * @brief Processes one keyboard scancode and updates the name buffer.
 *
 * Handles letter keys, Backspace, and Enter. All other scancodes are ignored.
 *
 * @param scancode Raw keyboard scancode (set 1 make code).
 * @return true when the player presses Enter to confirm the name.
 */
bool nameentry_handle_key(uint8_t scancode);

/**
 * @brief Returns a pointer to the current null-terminated name string.
 *
 * @return Pointer to the internal name buffer. Valid until the next call
 *         to nameentry_reset().
 */
const char *nameentry_get_name(void);

/**
 * @brief Draws the name being typed at the given screen position.
 *
 * @param x     Left edge of the text in screen pixels.
 * @param y     Top  edge of the text in screen pixels.
 * @param color Text colour in AARRGGBB format.
 */
void nameentry_draw(uint16_t x, uint16_t y, uint32_t color);

#endif /* NAMEENTRY_H */
