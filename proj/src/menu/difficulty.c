#include "difficulty.h"
#include "menu_nav.h"
#include "../font/font.h"
#include "video.h"

static difficulty_t selected = DIFF_EASY;

static const char *diff_labels[DIFF_OPTION_COUNT] = {
    "easy",
    "medium",
    "hard"
};

static void fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color) {
    for (uint16_t dy = 0; dy < h; dy++)
        for (uint16_t dx = 0; dx < w; dx++)
            vg_draw_pixel(x + dx, y + dy, color);
}

difficulty_t diff_get_selected(void) {
    return selected;
}

void diff_reset(void) {
    selected = DIFF_EASY;
}

bool diff_handle_key(uint8_t scancode) {
    return menu_nav_handle_key(scancode, (int *)&selected, DIFF_OPTION_COUNT);
}

void diff_draw(void) {
    uint16_t sw = (uint16_t) vg_get_x_res();
    uint16_t sh = (uint16_t) vg_get_y_res();

    // Dark background
    fill_rect(0, 0, sw, sh, 0xFF000000);

    // Title "select difficulty"
    const char *title = "select difficulty";
    uint16_t tw = font_string_width(title);
    uint16_t tx = (sw - tw) / 2;
    uint16_t ty = sh / 4;
    font_draw_string(tx, ty, title, 0xFFFFFFFF);

    // Options
    uint16_t item_h   = 16;
    uint16_t item_gap = item_h + 20;
    uint16_t menu_total = DIFF_OPTION_COUNT * item_gap - 20;
    uint16_t menu_top   = sh / 2 - menu_total / 2;

    for (int i = 0; i < DIFF_OPTION_COUNT; i++) {
        uint16_t iw = font_string_width(diff_labels[i]);
        uint16_t ix = (sw - iw) / 2;
        uint16_t iy = menu_top + (uint16_t)(i * item_gap);
        menu_nav_draw_item(ix, iy, diff_labels[i], i == (int)selected);
    }

    // Hint
    const char *hint = "esc - back";
    uint16_t hw = font_string_width(hint);
    font_draw_string((sw - hw) / 2, sh - 30, hint, 0xFF606070);
}
