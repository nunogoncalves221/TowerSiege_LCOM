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

// keyboard state (added spacebar for attacking)
static bool key_w = false;
static bool key_a = false;
static bool key_s = false;
static bool key_d = false;
static bool key_space = false;

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

    tower_init();
    map_init();
    hero_init();

    // setup cursor
    cursor_sprite = create_sprite((xpm_map_t) cursor);
    cursor_x = vg_get_x_res() / 2;
    cursor_y = vg_get_y_res() / 2; // start cursor in center of screen

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
               
                player_take_damage(1);
                
                if (player_is_dead()) {
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
                    player_add_score(10);
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
    hud_draw(&start_time);

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
