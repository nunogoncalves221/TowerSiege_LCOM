#include "game.h"
#include "sprite.h"
#include "video.h"
#include "map.h"
#include "enemy.h"
#include "../assets/hero.xpm"

#define MAX_ENEMIES 20 //ready for waves

static sprite_t *hero_sprite = NULL;
static int16_t hero_x = 0;
static int16_t hero_y = 0;

// array to manage multiple enemies
static enemy_t *enemies[MAX_ENEMIES];
static int enemy_count = 0;

static bool key_w = false;
static bool key_a = false;
static bool key_s = false;
static bool key_d = false;

void game_reset(void) {
    map_init();

    destroy_sprite(hero_sprite);
    hero_sprite = create_sprite((xpm_map_t) hero);

    if (hero_sprite != NULL) {
        hero_x = (int16_t)((vg_get_x_res() - hero_sprite->width)  / 2);
        hero_y = (int16_t)((vg_get_y_res() - hero_sprite->height) / 2);
    }

    // clean up any remaining enemies from previous games
    for (int i = 0; i < enemy_count; i++) {
        enemy_destroy(enemies[i]);
    }
    enemy_count = 0;

    // center enemies vertically relative to the 96px tile height using the variables
    int16_t sprite_w = (int16_t)hero_sprite->width;
    int16_t start_y = (int16_t)(TILE_SIZE * 1 + (TILE_SIZE / 2) - (hero_sprite->height / 2));

    // using sprite_w and start_y 
    enemies[0] = enemy_create((xpm_map_t) hero, -sprite_w,       start_y, 100, 2);
    enemies[1] = enemy_create((xpm_map_t) hero, -sprite_w - 150, start_y, 100, 3); 
    enemies[2] = enemy_create((xpm_map_t) hero, -sprite_w - 300, start_y, 100, 2);
    enemy_count = 3;
    
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

    // enemies manager & active memory cleanup
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i] != NULL) {
            enemy_update(enemies[i]);

            // if the enemy reached the end or died, destroy it
            if (!enemies[i]->active) {
                enemy_destroy(enemies[i]);
                
                // shift the remaining enemies in the array to keep it compact
                for (int j = i; j < enemy_count - 1; j++) {
                    enemies[j] = enemies[j + 1];
                }
                enemies[enemy_count - 1] = NULL;
                enemy_count--;
                i--; // re-check this index since a new enemy shifted into it
            }
        }
    }
}

void game_draw(void) {
    map_draw();

    // draw all active enemies
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i] != NULL) {
            enemy_draw(enemies[i]);
        }
    }

    if (hero_sprite == NULL) return;
    draw_sprite(hero_sprite, hero_x, hero_y);
}
