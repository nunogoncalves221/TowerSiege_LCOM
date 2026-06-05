#include "menu.h"
#include "menu_nav.h"
#include "../sprite/sprite.h"
#include "../font/font.h"
#include "video.h"
#include "../assets/menus/logo_tower_siege.xpm"

#define ESC_BREAKCODE  0x81

static menu_option_t selected = MENU_PLAY;
static sprite_t *logo_sprite  = NULL;

static const char *option_labels[MENU_OPTION_COUNT] = {
    "play",
    "instructions",
    "high scores",
    "quit"
};

static void fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color) {
    for (uint16_t dy = 0; dy < h; dy++)
        for (uint16_t dx = 0; dx < w; dx++)
            vg_draw_pixel(x + dx, y + dy, color);
}

static void draw_sprite_scaled(sprite_t *sp,
                                uint16_t dst_x, uint16_t dst_y,
                                uint16_t dst_w, uint16_t dst_h) {
    if (sp == NULL || sp->map == NULL) return;
    uint32_t transparent = xpm_transparency_color(XPM_8_8_8_8);
    uint16_t sw = (uint16_t) vg_get_x_res();
    uint16_t sh = (uint16_t) vg_get_y_res();

    // Map destination row back to source row
    for (uint16_t dy = 0; dy < dst_h; dy++) {
        uint16_t sy = (uint16_t)((uint32_t)dy * sp->height / dst_h);
        for (uint16_t dx = 0; dx < dst_w; dx++) {
            // Map destination column back to source column
            uint16_t sx = (uint16_t)((uint32_t)dx * sp->width / dst_w);
            uint32_t color = sp->map[(uint32_t)sy * sp->width + sx];

            if (color == transparent) continue;
            // Clamp to screen bounds before drawing
            uint16_t px = dst_x + dx;
            uint16_t py = dst_y + dy;
            if (px < sw && py < sh)
                vg_draw_pixel(px, py, color);
        }
    }
}

void menu_reset(void) {
    selected = MENU_PLAY;
    if (logo_sprite == NULL)
        logo_sprite = create_sprite((xpm_map_t) tower_siege_xpm);
}

menu_option_t menu_get_selected(void) {
    return selected;
}

bool menu_handle_key(uint8_t scancode) {
    return menu_nav_handle_key(scancode, (int *)&selected, MENU_OPTION_COUNT);
}

void menu_draw(void) {
    uint16_t sw = (uint16_t) vg_get_x_res();
    uint16_t sh = (uint16_t) vg_get_y_res();

    fill_rect(0, 0, sw, sh, 0xFF000000);

    uint16_t logo_area_h = sh / 3;

    if (logo_sprite != NULL) {
        //Scale the logo to fill the area while preserving aspect ratio
        uint16_t pad_y = logo_area_h / 6;
        uint16_t logo_h = logo_area_h - 2 * pad_y;
        uint16_t logo_w = (uint16_t)((uint32_t)logo_h * logo_sprite->width / logo_sprite->height);
        if (logo_w > sw) logo_w = sw;

        //Center horizontally
        uint16_t logo_x = (sw - logo_w) / 2;
        draw_sprite_scaled(logo_sprite, logo_x, pad_y, logo_w, logo_h);
    }

    // Options are distributed evenly in the bottom two thirds
    uint16_t item_h     = 16;
    uint16_t item_gap   = item_h + 16;
    uint16_t menu_total = MENU_OPTION_COUNT * item_gap - 16;
    uint16_t menu_top   = logo_area_h + (sh - logo_area_h - menu_total) / 2;

    for (int i = 0; i < MENU_OPTION_COUNT; i++) {
        uint16_t iw = font_string_width(option_labels[i]);
        uint16_t ix = (sw - iw) / 2;
        uint16_t iy = menu_top + (uint16_t)(i * item_gap);
        menu_nav_draw_item(ix, iy, option_labels[i], i == (int)selected);
    }
}
