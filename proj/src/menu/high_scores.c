#include "high_scores.h"
#include "../font/font.h"
#include "video.h"
#include <string.h>
#include <stdio.h>

#define COLOR_BG    0xFF000000
#define COLOR_TEXT  0xFFB0B0C0
#define COLOR_TITLE 0xFFE94560

#define SAVE_PATH "/home/lcom/labs/grupo_2leic16_2/proj/highscores.bin"

static hs_entry_t entries[HS_MAX_ENTRIES];

static void fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color) {
    for (uint16_t dy = 0; dy < h; dy++)
        for (uint16_t dx = 0; dx < w; dx++)
            vg_draw_pixel(x + dx, y + dy, color);
}

void hs_load(void) {
    memset(entries, 0, sizeof(entries)); // clear entries
    FILE *f = fopen(SAVE_PATH, "rb");
    if (!f) return; // no high scores
    fread(entries, sizeof(hs_entry_t), HS_MAX_ENTRIES, f); // read entries
    fclose(f);
}

void hs_save(void) {
    FILE *f = fopen(SAVE_PATH, "wb");
    if (!f) return;
    fwrite(entries, sizeof(hs_entry_t), HS_MAX_ENTRIES, f);
    fclose(f);
}

bool hs_qualifies(int score) {
    for (int i = 0; i < HS_MAX_ENTRIES; i++)
        if (score > entries[i].score) return true;
    return false;
}

void hs_insert(const char *name, int score, rtc_date date) {
    int pos = HS_MAX_ENTRIES;
    for (int i = 0; i < HS_MAX_ENTRIES; i++) {
        if (score > entries[i].score) { pos = i; break; }
    }
    if (pos == HS_MAX_ENTRIES) return;
    for (int i = HS_MAX_ENTRIES - 1; i > pos; i--)
        entries[i] = entries[i - 1];
    strncpy(entries[pos].name, name, HS_MAX_NAME_LEN);
    entries[pos].name[HS_MAX_NAME_LEN] = '\0';
    entries[pos].score = score;
    entries[pos].date  = date;
    hs_save();
}

const hs_entry_t *hs_get_entries(void) {
    return entries;
}

// draw helpers

static void int_to_str(int n, char *buf) {
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    char tmp[12]; int ti = 0;
    while (n > 0) { tmp[ti++] = '0' + n % 10; n /= 10; }
    for (int i = 0; i < ti; i++) buf[i] = tmp[ti - 1 - i];
    buf[ti] = '\0';
}

static void append_padded2(char *buf, int *pos, uint8_t n) {
    buf[(*pos)++] = '0' + n / 10;
    buf[(*pos)++] = '0' + n % 10;
}

static void append_spaces(char *buf, int *pos) {
    for (int i = 0; i < 3; i++) buf[(*pos)++] = ' ';
}

void hs_draw(void) {
    uint16_t sw = (uint16_t)vg_get_x_res();
    uint16_t sh = (uint16_t)vg_get_y_res();

    fill_rect(0, 0, sw, sh, COLOR_BG);

    uint16_t line = 25;
    uint16_t gap  = 10;
    uint16_t total_h = line                    // title
                     + 6 * gap                 // gap after title
                     + HS_MAX_ENTRIES * line   // high scores lines
                     + 4 * line                // gap before footer
                     + 10 * gap;               // footer

    uint16_t y = (sh - total_h) / 2;

    // title
    font_draw_string(sw / 2 - font_string_width("high scores") / 2,
                     y, "high scores", COLOR_TITLE);
    y += line + 6 * gap;

    // center table
    uint16_t char_w  = font_string_width("a");
    uint16_t block_w = (1 + 3 + 5 + 3 + 8 + 3 + 5 + 3 + HS_MAX_NAME_LEN) * char_w; // rank + score + date + name (with spacing)
    uint16_t left_x  = (sw - block_w) / 2;
    uint16_t rank_w  = 4 * char_w; // width of rank + spacing

    for (int i = 0; i < HS_MAX_ENTRIES; i++) {
        // red rank number
        char rank_buf[8];
        int_to_str(i + 1, rank_buf);
        uint16_t rank_x = left_x + char_w - font_string_width(rank_buf); // where to draw ranking number
        font_draw_string(rank_x, y, rank_buf, COLOR_TITLE);

        // grey value
        if (entries[i].name[0] != '\0') {
            char buf[64]; int p = 0;

            // score
            char score_buf[12];
            int_to_str(entries[i].score, score_buf);
            for (int j = 0; score_buf[j]; j++) buf[p++] = score_buf[j];

            // date (dd/mm/yy)
            append_spaces(buf, &p);
            append_padded2(buf, &p, entries[i].date.day);
            buf[p++] = '/';
            append_padded2(buf, &p, entries[i].date.month);
            buf[p++] = '/';
            append_padded2(buf, &p, entries[i].date.year);

            // time (hh:mm)
            append_spaces(buf, &p);
            append_padded2(buf, &p, entries[i].date.hours);
            buf[p++] = ':';
            append_padded2(buf, &p, entries[i].date.minutes);

            // name
            append_spaces(buf, &p);
            for (int j = 0; entries[i].name[j] && j < HS_MAX_NAME_LEN; j++)
                buf[p++] = entries[i].name[j];

            buf[p] = '\0';
            
            // draw the line
            font_draw_string(left_x + rank_w, y, buf, COLOR_TEXT);
        }

        y += line;
    }

    // footer
    y += 10 * gap;
    font_draw_string(sw / 2 - font_string_width("press esc to go back") / 2,
                     y, "press esc to go back", COLOR_TEXT);
}
