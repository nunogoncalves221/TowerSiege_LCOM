#include <stdio.h>

#include "game.h"
#include "sprite.h"
#include "animated_sprite.h"
#include "video.h"
#include "map.h"
#include "hero.h"
#include "enemy.h"
#include "enemy_manager.h"
#include "wave.h"
#include "tower.h"
#include "bullet.h"
#include "font.h"
#include "rtc.h"
#include "player.h"
#include "hud.h"

#include "../assets/cursor.xpm"

// cursor
static sprite_t *cursor_sprite = NULL;
static int16_t cursor_x = 0;
static int16_t cursor_y = 0;

// array to manage multiple enemies
static enemy_t *enemies[MAX_ENEMIES];
static int enemy_count = 0;

// keyboard state (added f for attacking)
static bool key_w = false;
static bool key_a = false;
static bool key_s = false;
static bool key_d = false;
static bool key_f = false;

// core game state
static bool game_over = false;

// rtc
static rtc_date start_time;

void game_reset(void) {
    // clean up old resources
    hero_destroy();
    for (int i = 0; i < enemy_count; i++) {
        enemy_destroy(enemies[i]);
    }
    enemy_count = 0;
    destroy_sprite(cursor_sprite);

    player_init();
    tower_init();
    map_init();
    hero_init();

    // setup cursor
    cursor_sprite = create_sprite((xpm_map_t) cursor);
    cursor_x = vg_get_x_res() / 2;
    cursor_y = vg_get_y_res() / 2; // start cursor in center of screen

    game_over = false;
    wave_init();
    key_w = key_a = key_s = key_d = key_f = false;

    // read the exact time the match started
    rtc_read_date(&start_time);
}

void game_handle_key(uint8_t scancode) {
    if (tower_popup_handle_key(scancode)) return;

    switch (scancode) {
        case KEY_W_MAKE:  key_w = true;  break;
        case KEY_W_BREAK: key_w = false; break;
        case KEY_A_MAKE:  key_a = true;  break;
        case KEY_A_BREAK: key_a = false; break;
        case KEY_S_MAKE:  key_s = true;  break;
        case KEY_S_BREAK: key_s = false; break;
        case KEY_D_MAKE:  key_d = true;  break;
        case KEY_D_BREAK: key_d = false; break;
        case KEY_F_MAKE: key_f = true; break;
        case KEY_F_BREAK: key_f = false; break; 

        default: break;
    }

    hero_update_input(key_w, key_a, key_s, key_d, key_f);
}

void game_tick(void) {
    hero_tick(enemies, enemy_count);

    if (enemy_manager_tick(enemies, &enemy_count)) game_over = true;
    
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

    enemy_manager_draw(enemies, enemy_count);  // dynamic elements are never cached
    bullet_draw();
    hero_draw();
    hud_draw(&start_time);

    // popup drawn over everything except the cursor
    tower_popup_draw();

    // cursor is always drawn last
    if (cursor_sprite != NULL) {
        draw_sprite(cursor_sprite, cursor_x, cursor_y);
    }
}

bool game_is_over(void) {
    return game_over;
}

int game_get_score(void) {
    return player_get_score();
}

int16_t game_get_cursor_y(void) {
    return cursor_y;
}

rtc_date game_get_start_time(void) {
    return start_time;
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
    // use pixel-accurate handler so the popup buttons work correctly
    tower_handle_click_px(cursor_x, cursor_y);
}
