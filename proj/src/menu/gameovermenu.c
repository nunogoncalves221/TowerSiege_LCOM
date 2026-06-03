#include "gameovermenu.h"
#include "menu_nav.h"
#include "../font/font.h"
#include "video.h"

#include <stdio.h>

#define GO_ITEM_GAP   32
#define GO_FONT_SCALE 3
#define GO_TITLE_H    48  // 16px * scale 3

static gameover_option_t go_selected = GAMEOVER_RETRY;

static const char *go_labels[GAMEOVER_OPTION_COUNT] = { "retry", "menu" };

static void fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color) {
    for (uint16_t dy = 0; dy < h; dy++)
        for (uint16_t dx = 0; dx < w; dx++)
            vg_draw_pixel(x + dx, y + dy, color);
}

static uint16_t get_menu_top(uint16_t sh) {
    uint16_t title_area_h = sh / 3;
    uint16_t menu_total   = GAMEOVER_OPTION_COUNT * GO_ITEM_GAP - 16;
    return title_area_h + (sh - title_area_h - menu_total) / 2;
}

void gameover_reset(void) {
    go_selected = GAMEOVER_RETRY;
}

gameover_option_t gameover_get_selected(void) {
    return go_selected;
}

bool gameover_handle_key(uint8_t scancode) {
    return menu_nav_handle_key(scancode, (int *)&go_selected, GAMEOVER_OPTION_COUNT);
}

void gameover_draw(int score) {
    uint16_t sw = (uint16_t) vg_get_x_res();
    uint16_t sh = (uint16_t) vg_get_y_res();

    fill_rect(0, 0, sw, sh, 0xFF000000);

    // "game over" title centred in the upper third
    uint16_t title_area_h = sh / 3;
    const char *title = "game over";
    uint16_t title_w = font_string_width_scaled(title, GO_FONT_SCALE);
    uint16_t title_x = (sw - title_w) / 2;
    uint16_t title_y = (title_area_h - GO_TITLE_H) / 2;
    font_draw_string_scaled(title_x + 3, title_y + 3, title, 0xFF660000, GO_FONT_SCALE);
    font_draw_string_scaled(title_x,     title_y,     title, 0xFFFF3333, GO_FONT_SCALE);

    // score just below the title area
    char score_str[32];
    snprintf(score_str, sizeof(score_str), "score %d", score);
    uint16_t sc_w = font_string_width(score_str);
    font_draw_string((sw - sc_w) / 2, title_area_h + 10, score_str, 0xFFFFFFFF);

    // options centred in the remaining space (same rhythm as main menu)
    uint16_t menu_top = get_menu_top(sh);
    for (int i = 0; i < GAMEOVER_OPTION_COUNT; i++) {
        uint16_t iw = font_string_width(go_labels[i]);
        uint16_t ix = (sw - iw) / 2;
        uint16_t iy = menu_top + (uint16_t)(i * GO_ITEM_GAP);
        menu_nav_draw_item(ix, iy, go_labels[i], i == (int)go_selected);
    }
}

int gameover_click(int16_t cursor_y) {
    uint16_t sh       = (uint16_t) vg_get_y_res();
    uint16_t menu_top = get_menu_top(sh);
    for (int i = 0; i < GAMEOVER_OPTION_COUNT; i++) {
        int iy = menu_top + i * GO_ITEM_GAP;
        if (cursor_y >= iy - 4 && cursor_y < iy + 20)
            return i + 1; // 1=retry, 2=menu
    }
    return 0;
}
