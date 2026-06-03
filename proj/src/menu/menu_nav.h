#ifndef MENU_NAV_H
#define MENU_NAV_H

#include <stdint.h>
#include <stdbool.h>

#define MENU_NAV_COLOR_SELECTED 0xFFE94560
#define MENU_NAV_COLOR_NORMAL   0xFFB0B0C0

// Returns true if ENTER was pressed; updates *selected on UP/DOWN.
bool menu_nav_handle_key(uint8_t scancode, int *selected, int count);

// Draws one option with selection dot and colour at (ix, iy).
void menu_nav_draw_item(uint16_t ix, uint16_t iy, const char *label, bool is_selected);

#endif
