/**
 * @file menu_nav.h
 * @brief Reusable keyboard navigation and item rendering for all menus.
 *
 * Provides a generic UP/DOWN/Enter handler and a single-item draw function
 * used by every menu screen to keep navigation behaviour consistent.
 */

#ifndef MENU_NAV_H
#define MENU_NAV_H

#include <stdint.h>
#include <stdbool.h>

#define MENU_NAV_COLOR_SELECTED 0xFFE94560
#define MENU_NAV_COLOR_NORMAL   0xFFB0B0C0

/**
 * @brief Handles UP, DOWN, and Enter keys for a generic option list.
 *
 * Decrements or increments *selected on UP/DOWN, wrapping around the list.
 *
 * @param scancode Raw keyboard scancode (set 1 make code).
 * @param selected Pointer to the current selection index; updated in place.
 * @param count    Total number of options in the list.
 * @return true if the player pressed Enter to confirm the current selection.
 */
bool menu_nav_handle_key(uint8_t scancode, int *selected, int count);

/**
 * @brief Draws a single menu option with a selection indicator.
 *
 * Renders @p label at (@p ix, @p iy). If @p is_selected is true the text
 * is drawn in ::MENU_NAV_COLOR_SELECTED with a dot indicator; otherwise it
 * uses ::MENU_NAV_COLOR_NORMAL.
 *
 * @param ix          Horizontal screen position in pixels.
 * @param iy          Vertical   screen position in pixels.
 * @param label       Null-terminated option label to render.
 * @param is_selected true if this option is currently highlighted.
 */
void menu_nav_draw_item(uint16_t ix, uint16_t iy, const char *label, bool is_selected);

#endif /* MENU_NAV_H */
