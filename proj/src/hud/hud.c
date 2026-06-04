#include <stdio.h>
#include "hud.h"
#include "video.h"
#include "font.h"
#include "player.h"
#include "wave.h"

// rtc ui constants
#define RTC_STR_MAX_LEN 64
#define RTC_UI_PAD_X 10
#define RTC_UI_PAD_Y 30
#define RTC_TEXT_COLOR 0x00FFFFFF // white text
#define RTC_BG_COLOR   0x00000000 // black background
#define RTC_FONT_H     14         // standard height of our pixel font
#define RTC_BG_PAD     4          // padding around the text box 

// general ui hp bar colors
#define UI_HP_COLOR_GOOD   0x0000FF00 // green
#define UI_HP_COLOR_BG     0x00FF0000 // red
#define UI_HP_COLOR_BORDER 0x00000000 // black

// general layout paddings
#define HUD_PAD_X 10
#define HUD_PAD_Y 10
#define HUD_WAVE_Y 30

// base hp bar dimensions
#define BASE_HP_BAR_W 200
#define BASE_HP_BAR_H 14

// string buffer limits
#define STR_BUF_SHORT 16
#define STR_BUF_LONG 32

// font colors
#define HUD_COLOR_TEXT 0x00FFFFFF // white for standard text
#define HUD_COLOR_GOLD 0x00FFD700 // yellow for the gold

void draw_hp_bar(int x, int y, int width, int height, int current, int max,
                 uint32_t color_good, uint32_t color_bg, uint32_t color_border) {
    int fill_w = (current * width) / max;
    int scr_w = (int)vg_get_x_res();
    int scr_h = (int)vg_get_y_res();
    for (int py = y - 1; py <= y + height; py++) {
        for (int px = x - 1; px <= x + width; px++) {
            if (px < 0 || px >= scr_w || py < 0 || py >= scr_h) continue;
            if (px == x - 1 || px == x + width || py == y - 1 || py == y + height)
                vg_draw_pixel(px, py, color_border);
            else
                vg_draw_pixel(px, py, px < x + fill_w ? color_good : color_bg);
        }
    }
}

void hud_draw(rtc_date *start_time) {
    // base lives bar
    draw_hp_bar(HUD_PAD_X, HUD_PAD_Y, BASE_HP_BAR_W, BASE_HP_BAR_H, player_get_lives(), player_get_max_lives(),
                UI_HP_COLOR_GOOD, UI_HP_COLOR_BG, UI_HP_COLOR_BORDER);

    // wave counter 
    char wave_str[STR_BUF_SHORT];
    snprintf(wave_str, sizeof(wave_str), "wave %d", wave_get_number());
    font_draw_string(HUD_PAD_X, HUD_WAVE_Y, wave_str, HUD_COLOR_TEXT);

    // score 
    char score_str[STR_BUF_LONG];
    snprintf(score_str, sizeof(score_str), "score %d", player_get_score());
    uint16_t stw = font_string_width(score_str);
    font_draw_string((uint16_t)((vg_get_x_res() - stw) / 2), HUD_PAD_Y, score_str, HUD_COLOR_TEXT);

    // gold 
    char gold_str[STR_BUF_LONG];
    snprintf(gold_str, sizeof(gold_str), "gold %d", player_get_gold());
    uint16_t tw = font_string_width(gold_str);
    font_draw_string((uint16_t)(vg_get_x_res() - tw - HUD_PAD_X), HUD_PAD_Y, gold_str, HUD_COLOR_GOLD);

    // rtc clock 
    char time_str[RTC_STR_MAX_LEN];
    snprintf(time_str, sizeof(time_str), "%02d %02d 20%02d   %02d %02d", 
             start_time->day, start_time->month, start_time->year, 
             start_time->hours, start_time->minutes);
             
    uint16_t time_w = font_string_width(time_str);
    uint16_t rtc_x = RTC_UI_PAD_X;
    uint16_t rtc_y = (uint16_t)(vg_get_y_res() - RTC_UI_PAD_Y);
    
    // black background
    for (int py = rtc_y - RTC_BG_PAD; py < rtc_y + RTC_FONT_H + RTC_BG_PAD; py++) {
        for (int px = rtc_x - RTC_BG_PAD; px < rtc_x + time_w + RTC_BG_PAD; px++) {
            if (px >= 0 && px < (int)vg_get_x_res() && py >= 0 && py < (int)vg_get_y_res()) {
                vg_draw_pixel(px, py, RTC_BG_COLOR); 
            }
        }
    }
    
    font_draw_string(rtc_x, rtc_y, time_str, RTC_TEXT_COLOR);
}
