#include "pause.h"
#include "menu_nav.h"
#include "../font/font.h"
#include "video.h"

static pause_option_t selected = PAUSE_CONTINUE;

static const char *option_labels[PAUSE_OPTION_COUNT] = {
    "continue",
    "quit",
};

static void fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color) {
    for (uint16_t dy = 0; dy < h; dy++)
        for (uint16_t dx = 0; dx < w; dx++)
            vg_draw_pixel(x + dx, y + dy, color);
}

void pause_reset(void) {
    selected = PAUSE_CONTINUE;
}

pause_option_t pause_get_selected(void) {
    return selected;
}

bool pause_handle_key(uint8_t scancode) {
    return menu_nav_handle_key(scancode, (int *)&selected, PAUSE_OPTION_COUNT);
}

void pause_draw(void) {
    uint16_t sw = (uint16_t) vg_get_x_res();
    uint16_t sh = (uint16_t) vg_get_y_res();

    // Dark overlay over the game
    uint16_t box_w = 300;
    uint16_t box_h = 160;
    uint16_t box_x = (sw - box_w) / 2;
    uint16_t box_y = (sh - box_h) / 2;

    fill_rect(box_x, box_y, box_w, box_h, 0xFF111111);

    // Title
    uint16_t title_x = box_x + (box_w - font_string_width("paused")) / 2;
    font_draw_string(title_x, box_y + 20, "paused", MENU_NAV_COLOR_SELECTED);

    // Options
    uint16_t item_gap = 35;
    uint16_t start_y  = box_y + 65;

    for (int i = 0; i < PAUSE_OPTION_COUNT; i++) {
        uint16_t iw = font_string_width(option_labels[i]);
        uint16_t ix = box_x + (box_w - iw) / 2;
        uint16_t iy = start_y + (uint16_t)(i * item_gap);
        menu_nav_draw_item(ix, iy, option_labels[i], i == (int)selected);
    }
}
