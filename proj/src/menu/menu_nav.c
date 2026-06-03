#include "menu_nav.h"
#include "../font/font.h"
#include "video.h"

#define KEY_UP_MAKE    0x48
#define KEY_DOWN_MAKE  0x50
#define KEY_W_MAKE     0x11
#define KEY_S_MAKE     0x1F
#define KEY_ENTER_MAKE 0x1C

#define DOT_SIZE 6
#define DOT_PAD  14

bool menu_nav_handle_key(uint8_t scancode, int *selected, int count) {
    switch (scancode) {
        case KEY_UP_MAKE:
        case KEY_W_MAKE:
            if (*selected > 0) (*selected)--;
            break;
        case KEY_DOWN_MAKE:
        case KEY_S_MAKE:
            if (*selected < count - 1) (*selected)++;
            break;
        case KEY_ENTER_MAKE:
            return true;
        default:
            break;
    }
    return false;
}

void menu_nav_draw_item(uint16_t ix, uint16_t iy, const char *label, bool is_selected) {
    if (is_selected) {
        uint16_t dot_x = ix - DOT_PAD - DOT_SIZE;
        uint16_t dot_y = iy + 5;
        for (uint16_t dy = 0; dy < DOT_SIZE; dy++)
            for (uint16_t dx = 0; dx < DOT_SIZE; dx++)
                vg_draw_pixel(dot_x + dx, dot_y + dy, MENU_NAV_COLOR_SELECTED);
    }
    uint32_t color = is_selected ? MENU_NAV_COLOR_SELECTED : MENU_NAV_COLOR_NORMAL;
    font_draw_string(ix, iy, label, color);
}
