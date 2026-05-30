#include "game.h"
#include "sprite.h"
#include "video.h"
#include "map.h"
#include "enemy.h"
#include "tower.h"
#include "font.h"
#include <stdio.h>
#include "../assets/hero.xpm"
#include "../assets/orc.xpm"
#include "../assets/cursor.xpm"

#define MAX_ENEMIES 20 // ready for waves

static sprite_t *hero_sprite = NULL;
static int16_t hero_x = 0;
static int16_t hero_y = 0;

static sprite_t *cursor_sprite = NULL;
static int16_t cursor_x = 0;
static int16_t cursor_y = 0;

// array to manage multiple enemies
static enemy_t *enemies[MAX_ENEMIES];
static int enemy_count = 0;

static bool key_w = false;
static bool key_a = false;
static bool key_s = false;
static bool key_d = false;

static int player_gold = 500;

void game_reset(void) {
    tower_init();
    map_init();

    destroy_sprite(hero_sprite);
    hero_sprite = create_sprite((xpm_map_t) hero);

    destroy_sprite(cursor_sprite);
    cursor_sprite = create_sprite((xpm_map_t) cursor);
    cursor_x = vg_get_x_res() / 2;
    cursor_y = vg_get_y_res() / 2; // start cursor in center of screen

    if (hero_sprite != NULL) {
        hero_x = (int16_t)((vg_get_x_res() - hero_sprite->width)  / 2);
        hero_y = (int16_t)((vg_get_y_res() - hero_sprite->height) / 2);
    }

    // clean up any remaining enemies from previous games
    for (int i = 0; i < enemy_count; i++) {
        enemy_destroy(enemies[i]);
    }
    enemy_count = 0;

    // create the first enemy to load the sprite into memory
    enemies[0] = enemy_create((xpm_map_t) orc, 0, 0, 100, 2);

    // read the exact dimensions of the newly loaded enemy sprite
    int16_t enemy_w = (int16_t)enemies[0]->sp->width;
    int16_t enemy_h = (int16_t)enemies[0]->sp->height;

    // calculate perfect grid centering for whatever size the sprite is
    int16_t start_y = (int16_t)(TILE_SIZE * 1 + (TILE_SIZE / 2) - (enemy_h / 2));

    // update the first enemy's position and spawn the rest using the correct math
    enemies[0]->x = -enemy_w;
    enemies[0]->y = start_y;
    
    // replace this single test spawn with a proper automated wave spawner, TODO!!!!
    enemy_count = 1;

    player_gold = 500;
    key_w = key_a = key_s = key_d = false;
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
        default: break;
    }
}

void game_tick(void) {
    if (hero_sprite == NULL) return;

    // HERO MOVEMENT
    int16_t screen_w = (int16_t) vg_get_x_res();
    int16_t screen_h = (int16_t) vg_get_y_res();

    if (key_w) hero_y -= HERO_SPEED;
    if (key_s) hero_y += HERO_SPEED;
    if (key_a) hero_x -= HERO_SPEED;
    if (key_d) hero_x += HERO_SPEED;

    // clamp to screen bounds
    if (hero_x < 0) hero_x = 0;
    if (hero_y < 0) hero_y = 0;
    if (hero_x > screen_w - (int16_t) hero_sprite->width)
        hero_x = screen_w - (int16_t) hero_sprite->width;
    if (hero_y > screen_h - (int16_t) hero_sprite->height)
        hero_y = screen_h - (int16_t) hero_sprite->height;

    // ENEMY MANAGER
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i] != NULL) {
            enemy_update(enemies[i]);

            // deactivate enemy if it leaves the screen, this is done here to avoid
            // calling vg_get_x_res() for every enemy in every frame
            if (enemies[i]->x > screen_w || enemies[i]->y > screen_h ||
                enemies[i]->y < -(int16_t) enemies[i]->sp->height ||
                enemies[i]->x < -(int16_t) enemies[i]->sp->width) {
                enemies[i]->active = false;
            }

            // if the enemy is inactive (dead or off-screen), destroy it
            if (!enemies[i]->active) {
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
    tower_tick();
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

    if (hero_sprite != NULL) {
        draw_sprite(hero_sprite, hero_x, hero_y);
    }

    if (cursor_sprite != NULL) {
        draw_sprite(cursor_sprite, cursor_x, cursor_y);
    }

    char gold_str[32];
    snprintf(gold_str, sizeof(gold_str), "gold %d", player_gold);
    uint16_t tw = font_string_width(gold_str);
    font_draw_string((uint16_t)(vg_get_x_res() - tw - 10), 10, gold_str, 0x00FFD700);
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
    tower_handle_click(cursor_x / TILE_SIZE, cursor_y / TILE_SIZE, &player_gold);
}
