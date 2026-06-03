#include <stdio.h>

#include "game.h"
#include "sprite.h"
#include "animated_sprite.h"
#include "video.h"
#include "map.h"
#include "hero.h"
#include "enemy.h"
#include "wave.h"
#include "tower.h"
#include "bullet.h"
#include "font.h"
#include "rtc.h"

#include "../assets/hero.xpm"
#include "../assets/cursor.xpm"


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

// cursor
static sprite_t *cursor_sprite = NULL;
static int16_t cursor_x = 0;
static int16_t cursor_y = 0;

// array to manage multiple enemies
static enemy_t *enemies[MAX_ENEMIES];
static int enemy_count = 0;

// keyboard state (added spacebar for attacking)
static bool key_w = false;
static bool key_a = false;
static bool key_s = false;
static bool key_d = false;
static bool key_space = false;

// player economy
static int player_gold = 10000;
static int player_lives = MAX_LIVES;
static int player_score = 0;
static bool game_over = false;

// rtc
static rtc_date start_time;

// gold getter
int get_player_gold() {
    return player_gold;
}

// gold setter
void add_player_gold(int amount) {
    player_gold += amount;
    
    if (player_gold < 0) {
        player_gold = 0;
    }
}

void game_reset(void) {
    // clean up old resources
    hero_destroy();
    for (int i = 0; i < enemy_count; i++) {
        enemy_destroy(enemies[i]);
    }
    enemy_count = 0;
    destroy_sprite(cursor_sprite);

    tower_init();
    map_init();
    hero_init();

    // setup cursor
    cursor_sprite = create_sprite((xpm_map_t) cursor);
    cursor_x = vg_get_x_res() / 2;
    cursor_y = vg_get_y_res() / 2; // start cursor in center of screen

    // setup animated hero ( replace xpm later )
    xpm_map_t idle_frames[] = { (xpm_map_t) hero }; // using base for idle
    xpm_map_t walk_frames[] = { (xpm_map_t) hero_walk_1, (xpm_map_t) hero_walk_2, (xpm_map_t) hero_walk_3 };
    xpm_map_t atk_frames[]  = { (xpm_map_t) hero_atk_1, (xpm_map_t) hero_atk_2, (xpm_map_t) hero_atk_3 };

    // create the animations (frames array, number of frames, ticks per frame)
    hero_idle = create_animated_sprite(idle_frames, 1, 15);
    hero_walk = create_animated_sprite(walk_frames, 3, 10);
    hero_attack = create_animated_sprite(atk_frames, 3, 8);

    // spawn hero at the base
    hero_x = HERO_BASE_X;
    hero_y = HERO_BASE_Y;
    hero_hp = HERO_MAX_HP;
    hero_respawn_timer = 0;
    current_hero_state = HERO_IDLE;

    // clean up any remaining enemies from previous games
    for (int i = 0; i < enemy_count; i++) {
        enemy_destroy(enemies[i]);
    }
    enemy_count = 0;

    player_gold = 10000;
    player_lives = MAX_LIVES;
    player_score = 0;
    game_over = false;
    wave_init();
    key_w = key_a = key_s = key_d = key_space = false;

    // read the exact time the match started
    rtc_read_date(&start_time);
}

void game_handle_key(uint8_t scancode) {
    switch (scancode) {
        case KEY_W_MAKE:  key_w = true;  break;
        case KEY_W_BREAK: key_w = false; break;
        case KEY_A_MAKE:  key_a = true;  break;
        case KEY_A_BREAK: key_a = false; break;
        case KEY_S_MAKE:  key_s = true;  break;
        case KEY_S_BREAK: key_s = false; break;
        case KEY_D_MAKE:  key_d = true;  break;
        case KEY_D_BREAK: key_d = false; break;

        // spacebar for attacking
        case KEY_F_MAKE: key_space = true; break;
        case KEY_F_BREAK: key_space = false; break; 

        default: break;
    }

    hero_update_input(key_w, key_a, key_s, key_d, key_space);
}

void game_tick(void) {
    int16_t screen_w = (int16_t) vg_get_x_res();
    int16_t screen_h = (int16_t) vg_get_y_res();

    hero_tick(enemies, enemy_count);

     // --- ENEMY MOVEMENT ---
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i] != NULL) {
            enemy_update(enemies[i]);

            // enemy escaped through the right edge — costs a life
            if (enemies[i]->x > screen_w) {
                enemies[i]->active = false;
                player_lives--;
                if (player_lives <= 0) {
                    player_lives = 0;
                    game_over = true;
                }
            }
            // safety: deactivate if out of bounds in any other direction
            else if (enemies[i]->y > screen_h ||
                     enemies[i]->y < -(int16_t) enemies[i]->sp->height ||
                     enemies[i]->x < -(int16_t) enemies[i]->sp->width) {
                enemies[i]->active = false;
            }

            // if the enemy is inactive (dead or off-screen), destroy it
            if (!enemies[i]->active) {
                if (enemies[i]->hp <= 0)
                    player_score += 10;
                enemy_destroy(enemies[i]);

                // shift array to maintain strict visual depth order
                for (int j = i; j < enemy_count - 1; j++) {
                    enemies[j] = enemies[j + 1];
                }
                enemies[enemy_count - 1] = NULL;
                enemy_count--;
                i--; // re-check this index since a new enemy shifted into it
            }
        }
    }

    wave_tick(enemies, &enemy_count);

    tower_tick(enemies, enemy_count);
    bullet_tick();
}

void game_draw(void) {
    bool was_dirty = map_draw();

    if (was_dirty) {
        tower_draw();        // bake towers + shop into the cache
        map_update_cache();
    }

    // dynamic elements are never cached
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i] != NULL) {
            enemy_draw(enemies[i]);
        }
    }

    bullet_draw();

    hero_draw();

    // lives bar: filled rectangle that shrinks as lives are lost
    draw_hp_bar(10, 10, 200, 14, player_lives, MAX_LIVES,
                UI_HP_COLOR_GOOD, UI_HP_COLOR_BG, UI_HP_COLOR_BORDER);

    // wave counter below the bar
    char wave_str[16];
    snprintf(wave_str, sizeof(wave_str), "wave %d", wave_get_number());
    font_draw_string(10, 30, wave_str, 0x00FFFFFF);

    // score counter — top-centre
    char score_str[32];
    snprintf(score_str, sizeof(score_str), "score %d", player_score);
    uint16_t stw = font_string_width(score_str);
    font_draw_string((uint16_t)((vg_get_x_res() - stw) / 2), 10, score_str, 0x00FFFFFF);

    // gold counter — top-right
    char gold_str[32];
    snprintf(gold_str, sizeof(gold_str), "gold %d", player_gold);
    uint16_t tw = font_string_width(gold_str);
    font_draw_string((uint16_t)(vg_get_x_res() - tw - 10), 10, gold_str, 0x00FFD700);

    // rtc clock — bottom-left (showing match start time for testing)
    // format the time string with spaces
    char time_str[RTC_STR_MAX_LEN];
    snprintf(time_str, sizeof(time_str), "%02d %02d 20%02d   %02d %02d", 
             start_time.day, start_time.month, start_time.year, 
             start_time.hours, start_time.minutes);
             
    uint16_t time_w = font_string_width(time_str);
    
    // figure out bottom-left placement
    uint16_t rtc_x = RTC_UI_PAD_X;
    uint16_t rtc_y = (uint16_t)(vg_get_y_res() - RTC_UI_PAD_Y);
    
    // draw the dark background box first
    for (int py = rtc_y - RTC_BG_PAD; py < rtc_y + RTC_FONT_H + RTC_BG_PAD; py++) {
        for (int px = rtc_x - RTC_BG_PAD; px < rtc_x + time_w + RTC_BG_PAD; px++) {
            // always bound check to avoid screen overflow crashes
            if (px >= 0 && px < (int)vg_get_x_res() && py >= 0 && py < (int)vg_get_y_res()) {
                vg_draw_pixel(px, py, RTC_BG_COLOR); 
            }
        }
    }
    
    // slap the text on top of the background
    font_draw_string(rtc_x, rtc_y, time_str, RTC_TEXT_COLOR);

    // cursor is always drawn last, on top of everything
    if (cursor_sprite != NULL) {
        draw_sprite(cursor_sprite, cursor_x, cursor_y);
    }
}

bool game_is_over(void) {
    return game_over;
}

int game_get_score(void) {
    return player_score;
}

int16_t game_get_cursor_y(void) {
    return cursor_y;
}

void game_update_cursor(int16_t dx, int16_t dy) {
    cursor_x += dx;
    cursor_y -= dy;  // y is inverted
    
    // clamp to screen
    int16_t w = (int16_t) vg_get_x_res();
    int16_t h = (int16_t) vg_get_y_res();
    if (cursor_x < 0) cursor_x = 0;
    if (cursor_y < 0) cursor_y = 0;
    if (cursor_x >= w) cursor_x = w - 1;
    if (cursor_y >= h) cursor_y = h - 1;
}

void game_handle_click(void) {
    // let the tower manager handle the grid click
    tower_handle_click(cursor_x / TILE_SIZE, cursor_y / TILE_SIZE);
}
