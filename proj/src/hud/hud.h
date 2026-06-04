#ifndef _HUD_H_
#define _HUD_H_

#include <stdint.h>
#include "rtc.h"

// draws all the main ui elements (lives, score, gold, wave, match time)
void hud_draw(rtc_date *start_time);

// generic hp bar drawer shared across modules (used by the hero too)
void draw_hp_bar(int x, int y, int width, int height, int current, int max, uint32_t color_good, uint32_t color_bg, uint32_t color_border);

#endif
