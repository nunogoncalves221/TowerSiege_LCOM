#include "menu.h"
#include "../sprite/sprite.h"
#include "video.h"
#include "../assets/menus/logo_tower_siege.xpm"

// Keyboard scancodes 
#define KEY_UP_MAKE    0x48   // arrow up   (make)
#define KEY_DOWN_MAKE  0x50   // arrow down (make)
#define KEY_ENTER_MAKE 0x1C   // Enter      (make)
#define KEY_W_MAKE     0x11
#define KEY_S_MAKE     0x1F
#define ESC_BREAKCODE  0x81

// Colours (0xAARRGGBB in XPM_8_8_8_8 mode) 
#define COLOR_BG          0xFF0D0D0D   // near-black background
#define COLOR_TITLE_BG    0xFF1A1A2E   // dark navy behind logo
#define COLOR_SELECTED    0xFFE94560   // vivid red/pink for selected item
#define COLOR_NORMAL      0xFFB0B0C0   // muted silver for unselected items
#define COLOR_HINT        0xFF555566   // very dim hint text at bottom

// Menu state
static menu_option_t selected = MENU_PLAY;
static sprite_t *logo_sprite  = NULL;

// Option labels (drawn as pixel-art text via vg_draw_text if available,
//    otherwise we hand-draw each glyph as a filled rectangle "box".)
static const char *option_labels[MENU_OPTION_COUNT] = {
    "PLAY",
    "INSTRUCTIONS",
    "SETTINGS",
    "HIGH SCORES",
    "QUIT"
};

// Each character is 5 columns x 7 rows, stored as 5 bytes (one per column,
static const uint8_t font5x7[][5] = {
  /*  */ {0x00,0x00,0x00,0x00,0x00},
  /* ! */ {0x00,0x00,0x5F,0x00,0x00},
  /* " */ {0x00,0x07,0x00,0x07,0x00},
  /* # */ {0x14,0x7F,0x14,0x7F,0x14},
  /* $ */ {0x24,0x2A,0x7F,0x2A,0x12},
  /* % */ {0x23,0x13,0x08,0x64,0x62},
  /* & */ {0x36,0x49,0x55,0x22,0x50},
  /* ' */ {0x00,0x05,0x03,0x00,0x00},
  /* ( */ {0x00,0x1C,0x22,0x41,0x00},
  /* ) */ {0x00,0x41,0x22,0x1C,0x00},
  /* * */ {0x14,0x08,0x3E,0x08,0x14},
  /* + */ {0x08,0x08,0x3E,0x08,0x08},
  /* , */ {0x00,0x50,0x30,0x00,0x00},
  /* - */ {0x08,0x08,0x08,0x08,0x08},
  /* . */ {0x00,0x60,0x60,0x00,0x00},
  /* / */ {0x20,0x10,0x08,0x04,0x02},
  /* 0 */ {0x3E,0x51,0x49,0x45,0x3E},
  /* 1 */ {0x00,0x42,0x7F,0x40,0x00},
  /* 2 */ {0x42,0x61,0x51,0x49,0x46},
  /* 3 */ {0x21,0x41,0x45,0x4B,0x31},
  /* 4 */ {0x18,0x14,0x12,0x7F,0x10},
  /* 5 */ {0x27,0x45,0x45,0x45,0x39},
  /* 6 */ {0x3C,0x4A,0x49,0x49,0x30},
  /* 7 */ {0x01,0x71,0x09,0x05,0x03},
  /* 8 */ {0x36,0x49,0x49,0x49,0x36},
  /* 9 */ {0x06,0x49,0x49,0x29,0x1E},
  /* : */ {0x00,0x36,0x36,0x00,0x00},
  /* ; */ {0x00,0x56,0x36,0x00,0x00},
  /* < */ {0x08,0x14,0x22,0x41,0x00},
  /* = */ {0x14,0x14,0x14,0x14,0x14},
  /* > */ {0x00,0x41,0x22,0x14,0x08},
  /* ? */ {0x02,0x01,0x51,0x09,0x06},
  /* @ */ {0x32,0x49,0x79,0x41,0x3E},
  /* A */ {0x7E,0x11,0x11,0x11,0x7E},
  /* B */ {0x7F,0x49,0x49,0x49,0x36},
  /* C */ {0x3E,0x41,0x41,0x41,0x22},
  /* D */ {0x7F,0x41,0x41,0x22,0x1C},
  /* E */ {0x7F,0x49,0x49,0x49,0x41},
  /* F */ {0x7F,0x09,0x09,0x09,0x01},
  /* G */ {0x3E,0x41,0x49,0x49,0x7A},
  /* H */ {0x7F,0x08,0x08,0x08,0x7F},
  /* I */ {0x00,0x41,0x7F,0x41,0x00},
  /* J */ {0x20,0x40,0x41,0x3F,0x01},
  /* K */ {0x7F,0x08,0x14,0x22,0x41},
  /* L */ {0x7F,0x40,0x40,0x40,0x40},
  /* M */ {0x7F,0x02,0x0C,0x02,0x7F},
  /* N */ {0x7F,0x04,0x08,0x10,0x7F},
  /* O */ {0x3E,0x41,0x41,0x41,0x3E},
  /* P */ {0x7F,0x09,0x09,0x09,0x06},
  /* Q */ {0x3E,0x41,0x51,0x21,0x5E},
  /* R */ {0x7F,0x09,0x19,0x29,0x46},
  /* S */ {0x46,0x49,0x49,0x49,0x31},
  /* T */ {0x01,0x01,0x7F,0x01,0x01},
  /* U */ {0x3F,0x40,0x40,0x40,0x3F},
  /* V */ {0x1F,0x20,0x40,0x20,0x1F},
  /* W */ {0x3F,0x40,0x38,0x40,0x3F},
  /* X */ {0x63,0x14,0x08,0x14,0x63},
  /* Y */ {0x07,0x08,0x70,0x08,0x07},
  /* Z */ {0x61,0x51,0x49,0x45,0x43},
};


static void fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color) {
    for (uint16_t dy = 0; dy < h; dy++)
        for (uint16_t dx = 0; dx < w; dx++)
            vg_draw_pixel(x + dx, y + dy, color);
}

// Draw sprite scaled to exactly (dst_w x dst_h) using nearest-neighbour
static void draw_sprite_scaled(sprite_t *sp,
                                uint16_t dst_x, uint16_t dst_y,
                                uint16_t dst_w, uint16_t dst_h) {
    if (sp == NULL || sp->map == NULL) return;

    uint32_t transparent = xpm_transparency_color(XPM_8_8_8_8);
    uint16_t sw = (uint16_t) vg_get_x_res();
    uint16_t sh = (uint16_t) vg_get_y_res();

    for (uint16_t dy = 0; dy < dst_h; dy++) {
        // map destination row -> source row
        uint16_t sy = (uint16_t)((uint32_t)dy * sp->height / dst_h);
        for (uint16_t dx = 0; dx < dst_w; dx++) {
            uint16_t sx = (uint16_t)((uint32_t)dx * sp->width / dst_w);
            uint32_t color = sp->map[(uint32_t)sy * sp->width + sx];
            if (color == transparent) continue;
            uint16_t px = dst_x + dx;
            uint16_t py = dst_y + dy;
            if (px < sw && py < sh)
                vg_draw_pixel(px, py, color);
        }
    }
}

// Draw a single character at pixel position (x,y) with given scale and color
static void draw_char(uint16_t x, uint16_t y, char c, uint8_t scale, uint32_t color) {
    if (c < 32 || c > 90) c = 32; // clamp to table range
    const uint8_t *glyph = font5x7[c - 32];
    for (uint8_t col = 0; col < 5; col++) {
        for (uint8_t row = 0; row < 7; row++) {
            if (glyph[col] & (1 << row)) {
                fill_rect(x + col * scale, y + row * scale, scale, scale, color);
            }
        }
    }
}

// Draw a string; returns the total pixel width used
static uint16_t draw_string(uint16_t x, uint16_t y, const char *s, uint8_t scale, uint32_t color) {
    uint16_t cx = x;
    while (*s) {
        draw_char(cx, y, *s, scale, color);
        cx += (5 + 1) * scale; // 5px glyph + 1px spacing
        s++;
    }
    return cx - x;
}

// Measure string width in pixels
static uint16_t string_width(const char *s, uint8_t scale) {
    uint16_t n = 0;
    while (*s++) n++;
    return n > 0 ? (uint16_t)(n * (5 + 1) * scale - scale) : 0; // remove trailing gap
}

// Public API

void menu_reset(void) {
    selected = MENU_PLAY;
    if (logo_sprite == NULL)
        logo_sprite = create_sprite((xpm_map_t) tower_siege_xpm);
}

menu_option_t menu_get_selected(void) {
    return selected;
}

bool menu_handle_key(uint8_t scancode) {
    switch (scancode) {
        case KEY_UP_MAKE:
        case KEY_W_MAKE:
            if (selected > MENU_PLAY) selected--;
            break;

        case KEY_DOWN_MAKE:
        case KEY_S_MAKE:
            if (selected < MENU_OPTION_COUNT - 1) selected++;
            break;

        case KEY_ENTER_MAKE:
            return true;   // caller checks menu_get_selected()

        default:
            break;
    }
    return false;
}

void menu_draw(void) {
    uint16_t sw = (uint16_t) vg_get_x_res();
    uint16_t sh = (uint16_t) vg_get_y_res();

    fill_rect(0, 0, sw, sh, 0xFF000000);

    uint16_t logo_area_h = sh / 3;
    if (logo_sprite != NULL) {
        uint16_t pad_y = logo_area_h / 6;
        uint16_t logo_h = logo_area_h - 2 * pad_y;
        uint16_t logo_w = (uint16_t)((uint32_t)logo_h * logo_sprite->width / logo_sprite->height);
        if (logo_w > sw) logo_w = sw;
        uint16_t logo_x = (sw - logo_w) / 2;
        draw_sprite_scaled(logo_sprite, logo_x, pad_y, logo_w, logo_h);
    }

    uint8_t  item_scale = 3;
    uint16_t item_h     = 7 * item_scale;
    uint16_t item_gap   = item_h + 16;
    uint16_t menu_total = MENU_OPTION_COUNT * item_gap - 16;
    uint16_t menu_top   = logo_area_h + (sh - logo_area_h - menu_total) / 2;

    for (int i = 0; i < MENU_OPTION_COUNT; i++) {
        uint16_t iw    = string_width(option_labels[i], item_scale);
        uint16_t ix    = (sw - iw) / 2;
        uint16_t iy    = menu_top + (uint16_t)(i * item_gap);
        uint32_t color = (i == (int)selected) ? COLOR_SELECTED : COLOR_NORMAL;

        if (i == (int)selected) {
            uint16_t dot_x = ix - 14;
            uint16_t dot_y = iy + item_h / 2 - 3;
            fill_rect(dot_x, dot_y, 6, 6, COLOR_SELECTED);
        }

        draw_string(ix, iy, option_labels[i], item_scale, color);
    }
}
