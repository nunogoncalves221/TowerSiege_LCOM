#include "gameovermenu.h"
#include "menu_nav.h"
#include "../font/font.h"
#include "video.h"

static gameover_option_t selected = GAMEOVER_RETRY;

static const char *option_labels[GAMEOVER_OPTION_COUNT] = {
    "retry",
    "menu"
};

static void fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                      uint32_t color) {
    for (uint16_t dy = 0; dy < h; dy++)
        for (uint16_t dx = 0; dx < w; dx++)
            vg_draw_pixel(x + dx, y + dy, color);
}

void gameover_reset(void) {
    selected = GAMEOVER_RETRY;
}

gameover_option_t gameover_get_selected(void) {
    return selected;
}

bool gameover_handle_key(uint8_t scancode) {
    return menu_nav_handle_key(scancode, (int *)&selected,
                               GAMEOVER_OPTION_COUNT);
}

/* Converte um inteiro não-negativo em string (sem stdlib) */
static void int_to_str(int n, char *buf, int buf_size) {
    if (buf_size < 2) return;
    if (n < 0) n = 0;
    int i = buf_size - 1;
    buf[i] = '\0';
    if (n == 0) {
        buf[--i] = '0';
    } else {
        while (n > 0 && i > 0) {
            buf[--i] = '0' + (n % 10);
            n /= 10;
        }
    }
    /* move para o início do buffer */
    int j = 0;
    while (buf[i]) buf[j++] = buf[i++];
    buf[j] = '\0';
}

void gameover_draw(int score) {
    uint16_t sw = (uint16_t) vg_get_x_res();
    uint16_t sh = (uint16_t) vg_get_y_res();

    /* fundo preto — igual ao menu normal */
    fill_rect(0, 0, sw, sh, 0xFF000000);

    /* ── área do topo (1/3 do ecrã): título + score ── */
    uint16_t title_area_h = sh / 3;

    /* "game over" — escalonado 3x para se destacar */
    const char *title = "game over";
    uint8_t scale = 5;
    uint16_t title_w = font_string_width_scaled(title, scale);
    uint16_t title_x = (sw - title_w) / 2;
    uint16_t title_y = (title_area_h - scale * 16) / 2;
    font_draw_string_scaled(title_x, title_y, title,
                            MENU_NAV_COLOR_SELECTED, scale);

    /* "score: NNN" — tamanho normal, centrado abaixo do título */
    char score_buf[32];
    /* prefixo fixo + número */
    const char *prefix = "score  ";
    char num_buf[16];
    int_to_str(score, num_buf, (int)sizeof(num_buf));

    /* concatenar prefixo + número em score_buf */
    int pi = 0, ni = 0, si = 0;
    while (prefix[pi] && si < (int)sizeof(score_buf) - 1)
        score_buf[si++] = prefix[pi++];
    while (num_buf[ni] && si < (int)sizeof(score_buf) - 1)
        score_buf[si++] = num_buf[ni++];
    score_buf[si] = '\0';

    uint16_t score_w = font_string_width(score_buf);
    uint16_t score_x = (sw - score_w) / 2;
    uint16_t score_y = title_y + scale * 16 + 20; /* 3x glyph height + gap */
    font_draw_string(score_x, score_y, score_buf, MENU_NAV_COLOR_NORMAL);

    /* ── opções — centradas nos 2/3 inferiores (igual ao menu) ── */
    uint16_t item_h     = 16;
    uint16_t item_gap   = item_h + 16;
    uint16_t menu_total = GAMEOVER_OPTION_COUNT * item_gap - 16;
    uint16_t menu_top   = title_area_h +
                          (sh - title_area_h - menu_total) / 2;

    for (int i = 0; i < GAMEOVER_OPTION_COUNT; i++) {
        uint16_t iw = font_string_width(option_labels[i]);
        uint16_t ix = (sw - iw) / 2;
        uint16_t iy = menu_top + (uint16_t)(i * item_gap);
        menu_nav_draw_item(ix, iy, option_labels[i],
                           i == (int) selected);
    }
}
