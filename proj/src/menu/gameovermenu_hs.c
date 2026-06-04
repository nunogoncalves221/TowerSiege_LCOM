#include "gameovermenu_hs.h"
#include "nameentry.h"
#include "menu_nav.h"
#include "../font/font.h"
#include "video.h"
#include "high_scores.h"

#define COLOR_BG    0xFF000000
#define COLOR_TEXT  0xFFB0B0C0
#define COLOR_TITLE 0xFFE94560

static void fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color) {
    for (uint16_t dy = 0; dy < h; dy++)
        for (uint16_t dx = 0; dx < w; dx++)
            vg_draw_pixel(x + dx, y + dy, color);
}

static void int_to_str(int n, char *buf) {
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    char tmp[12]; int ti = 0;
    while (n > 0) { tmp[ti++] = '0' + n % 10; n /= 10; }
    for (int i = 0; i < ti; i++) buf[i] = tmp[ti - 1 - i];
    buf[ti] = '\0';
}

void gameover_hs_reset(void) {
    nameentry_reset();
}

bool gameover_hs_handle_key(uint8_t scancode) {
    return nameentry_handle_key(scancode);
}

void gameover_hs_draw(int score) {
    uint16_t sw = (uint16_t)vg_get_x_res();
    uint16_t sh = (uint16_t)vg_get_y_res();

    fill_rect(0, 0, sw, sh, COLOR_BG);

    uint16_t title_area_h = sh / 3;

    // "game over" title — same as normal gameover
    const char *title = "game over";
    uint8_t scale = 5;
    uint16_t title_w = font_string_width_scaled(title, scale);
    uint16_t title_y = (title_area_h - scale * 16) / 2;
    font_draw_string_scaled((sw - title_w) / 2, title_y, title,
                            MENU_NAV_COLOR_SELECTED, scale);

    // score
    char score_buf[32];
    char num_buf[16];
    int_to_str(score, num_buf);
    int si = 0;
    const char *prefix = "score  ";
    for (int i = 0; prefix[i]; i++) score_buf[si++] = prefix[i];
    for (int i = 0; num_buf[i];  i++) score_buf[si++] = num_buf[i];
    score_buf[si] = '\0';
    uint16_t score_w = font_string_width(score_buf);
    uint16_t score_y = title_y + scale * 16 + 20;
    font_draw_string((sw - score_w) / 2, score_y, score_buf, MENU_NAV_COLOR_NORMAL);

    // "new high score!"
    const char *hs_label = "new high score!";
    uint16_t hs_w = font_string_width(hs_label);
    uint16_t hs_y = score_y + 75;
    font_draw_string((sw - hs_w) / 2, hs_y, hs_label, COLOR_TITLE);

    // "enter your name:" prompt
    const char *prompt = "enter your name:";
    uint16_t prompt_w = font_string_width(prompt);
    uint16_t prompt_y = hs_y + 45;
    font_draw_string((sw - prompt_w) / 2, prompt_y, prompt, COLOR_TEXT);

    // name input field — centered based on max name length + cursor
    uint16_t char_w       = font_string_width("a");
    uint16_t name_field_w = (HS_MAX_NAME_LEN + 1) * char_w;
    uint16_t name_x       = (sw - name_field_w) / 2;
    uint16_t name_y       = prompt_y + 35;
    nameentry_draw(name_x, name_y, 0xFFFFFFFF);

    // hint — placed just below the name field, not pinned to the bottom
    const char *hint = "press enter to confirm";
    uint16_t hint_w = font_string_width(hint);
    font_draw_string((sw - hint_w) / 2, name_y + 80, hint, COLOR_TEXT);
}
