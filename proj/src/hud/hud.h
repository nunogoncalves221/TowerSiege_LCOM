/**
 * @file hud.h
 * @brief Heads-up display rendering for in-game UI elements.
 *
 * Draws the player's lives, score, gold, current wave number, and elapsed
 * match time. Also exposes a generic HP bar renderer shared by other modules.
 */

#ifndef _HUD_H_
#define _HUD_H_

#include <stdint.h>
#include "rtc.h"

/**
 * @brief Draws all HUD elements for the current frame.
 *
 * Renders lives, score, gold, wave number, and the time elapsed since the
 * match started.
 *
 * @param start_time Pointer to the RTC timestamp recorded at game start,
 *                   used to compute and display the match duration.
 */
void hud_draw(rtc_date *start_time);

/**
 * @brief Draws a filled HP bar with a coloured fill and border.
 *
 * @param x            Left edge of the bar in screen pixels.
 * @param y            Top  edge of the bar in screen pixels.
 * @param width        Total width of the bar in pixels.
 * @param height       Total height of the bar in pixels.
 * @param current      Current HP value (determines fill fraction).
 * @param max          Maximum HP value (full bar).
 * @param color_good   Fill colour in AARRGGBB format.
 * @param color_bg     Background colour for the empty portion.
 * @param color_border Border colour drawn around the bar.
 */
void draw_hp_bar(int x, int y, int width, int height, int current, int max,
                 uint32_t color_good, uint32_t color_bg, uint32_t color_border);

#endif /* _HUD_H_ */
