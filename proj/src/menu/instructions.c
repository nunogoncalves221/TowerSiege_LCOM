#include "instructions.h"
#include "../font/font.h"
#include "video.h"

#define COLOR_BG       0xFF000000
#define COLOR_TEXT     0xFFB0B0C0
#define COLOR_TITLE    0xFFE94560
#define COLOR_SELECTED 0xFFE94560

static void fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color) {
    for (uint16_t dy = 0; dy < h; dy++)
        for (uint16_t dx = 0; dx < w; dx++)
            vg_draw_pixel(x + dx, y + dy, color);
}

static void draw_centered(uint16_t y, const char *s, uint32_t color) {
    uint16_t sw = (uint16_t)vg_get_x_res();
    font_draw_string((uint16_t)(sw / 2 - font_string_width(s) / 2), y, s, color);
}

void instructions_draw(void) {
    uint16_t sw = (uint16_t)vg_get_x_res();
    uint16_t sh = (uint16_t)vg_get_y_res();
    fill_rect(0, 0, sw, sh, COLOR_BG);

    uint16_t line = 25, gap = 10;

    const char *objective[] = {
        "stop enemies from reaching the end of the path",
        "survive all waves to win",
    };
    const char *controls[] = {
        "w a s d       move hero",
        "f             hero attack",
        "left click    place or select tower",
        "arrows/w s    navigate tower menu",
        "enter         confirm",
        "esc           back to menu",
    };
    const char *towers[] = {
        "archer   cost -200 g   upgrade -250 g",
        "mage     cost -250 g   upgrade -300 g",
        "click a tower to upgrade or remove it",
        "removing returns 30 percent of total cost paid",
    };

    uint16_t total_h = line + gap             // title
                     + line + 2 * line + gap  // objective
                     + line + 6 * line + gap  // controls
                     + line + 4 * line + gap  // towers
                     + line;                  // footer

    uint16_t y = (sh - total_h) / 2;

    draw_centered(y, "instructions", COLOR_TITLE);   y += line + gap;

    draw_centered(y, "objective", COLOR_SELECTED);   y += line;
    for (int i = 0; i < 2; i++, y += line)
        draw_centered(y, objective[i], COLOR_TEXT);
    y += gap;

    draw_centered(y, "controls", COLOR_SELECTED);    y += line;
    for (int i = 0; i < 6; i++, y += line)
        draw_centered(y, controls[i], COLOR_TEXT);
    y += gap;

    draw_centered(y, "towers", COLOR_SELECTED);      y += line;
    for (int i = 0; i < 4; i++, y += line)
        draw_centered(y, towers[i], COLOR_TEXT);
    y += gap;

    draw_centered(y, "press esc to go back", COLOR_TEXT);
}
