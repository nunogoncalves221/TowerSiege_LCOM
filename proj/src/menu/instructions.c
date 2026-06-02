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

void instructions_draw(void) {
    uint16_t sw = (uint16_t) vg_get_x_res();
    uint16_t sh = (uint16_t) vg_get_y_res();

    fill_rect(0, 0, sw, sh, COLOR_BG);

    // Calculate total height to center content vertically
    uint16_t line = 25;
    uint16_t gap  = 10;
    uint16_t total_h = line            // title
                     + gap + line      // objective header
                     + 2 * line        // objective lines
                     + gap + line      // controls header
                     + 3 * line        // controls lines
                     + gap + line      // towers header
                     + 3 * line        // towers lines
                     + gap + line;     // footer

    uint16_t y = (sh - total_h) / 2;

    // Title
    font_draw_string(sw / 2 - font_string_width("instructions") / 2,
                     y, "instructions", COLOR_TITLE);
    y += line + gap;

    // Objective section
    font_draw_string(sw / 2 - font_string_width("objective") / 2,
                     y, "objective", COLOR_SELECTED);
    y += line;

    const char *obj[] = {
        "place towers to stop enemies",
        "from reaching the end of the path",
    };
    for (int i = 0; i < 2; i++, y += line)
        font_draw_string(sw / 2 - font_string_width(obj[i]) / 2, y, obj[i], COLOR_TEXT);

    // Controls section
    y += gap;
    font_draw_string(sw / 2 - font_string_width("controls") / 2,
                     y, "controls", COLOR_SELECTED);
    y += line;

    const char *controls[] = {
        "w a s d     move hero",
        "left click  place selected tower",
        "esc         return to menu",
    };
    for (int i = 0; i < 3; i++, y += line)
        font_draw_string(sw / 2 - font_string_width(controls[i]) / 2, y, controls[i], COLOR_TEXT);

    // Towers section
    y += gap;
    font_draw_string(sw / 2 - font_string_width("towers") / 2,
                     y, "towers", COLOR_SELECTED);
    y += line;

    const char *towers[] = {
        "archer  cost 200  range 200  damage 10",
        "mage    cost 350  range 150  damage 25",
        "select the tower at the bottom of the screen",
        "then left click a blank tile to place",
    };
    for (int i = 0; i < 3; i++, y += line)
        font_draw_string(sw / 2 - font_string_width(towers[i]) / 2, y, towers[i], COLOR_TEXT);

    // Footer
    y += gap * 3;
    font_draw_string(sw / 2 - font_string_width("press esc to go back") / 2,
                     y, "press esc to go back", COLOR_TEXT);
}
