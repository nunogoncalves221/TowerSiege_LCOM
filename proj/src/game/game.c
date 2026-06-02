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

#define MAX_ENEMIES 20
#define MAX_LIVES   20
#define WAVE_DELAY  150  // frames between waves

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
static int player_lives = MAX_LIVES;
static bool game_over = false;
static int go_selected = 0; // 0=retry, 1=menu

static const char *go_labels[2] = { "retry", "menu" };

// cached spawn position calculated once from sprite dimensions
static int16_t enemy_start_x = 0;
static int16_t enemy_start_y = 0;

// wave state
static int wave_number = 0;
static int enemies_to_spawn = 0;
static int spawn_timer = 0;
static int wave_delay = 0;

static void start_wave(int wave) {
    wave_number = wave;
    enemies_to_spawn = 5 + (wave - 1) * 3; // wave 1→5, 2→8, 3→11, …
    spawn_timer = 60; // 2-second lead-in before first enemy of each wave
    wave_delay = 0;
}

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

    // probe a temporary enemy to measure sprite dimensions, then discard it
    enemy_t *probe = enemy_create((xpm_map_t) orc, 0, 0, 100, 2);
    if (probe != NULL) {
        enemy_start_x = -(int16_t)probe->sp->width;
        enemy_start_y = (int16_t)(TILE_SIZE * 1 + (TILE_SIZE / 2) - (int16_t)(probe->sp->height / 2));
        enemy_destroy(probe);
    }

    player_gold = 500;
    player_lives = MAX_LIVES;
    game_over = false;
    start_wave(1);
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
    if (hero_sprite == NULL || game_over) return;

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

            // enemy escaped through the right edge — costs a life
            if (enemies[i]->x > screen_w) {
                enemies[i]->active = false;
                player_lives--;
                if (player_lives <= 0) {
                    player_lives = 0;
                    game_over = true;
                    go_selected = 0;
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

    // WAVE MANAGER
    if (enemies_to_spawn > 0) {
        // still releasing enemies for this wave
        spawn_timer--;
        if (spawn_timer <= 0 && enemy_count < MAX_ENEMIES) {
            enemy_t *e = enemy_create((xpm_map_t) orc, enemy_start_x, enemy_start_y, 100, 2);
            if (e != NULL) {
                enemies[enemy_count++] = e;
                enemies_to_spawn--;
                int interval = 90 - (wave_number - 1) * 5;
                spawn_timer = interval < 45 ? 45 : interval;
            }
        }
    } else if (enemy_count == 0) {
        // all enemies dealt with — count up to the inter-wave pause
        wave_delay++;
        if (wave_delay >= WAVE_DELAY) {
            start_wave(wave_number + 1);
        }
    }

    tower_tick(enemies, enemy_count);
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

    // lives bar: filled rectangle that shrinks as lives are lost
    int bar_x = 10, bar_y = 10;
    int bar_w = 200, bar_h = 14;
    int fill_w = (bar_w * player_lives) / MAX_LIVES;
    uint32_t fill_color = player_lives > MAX_LIVES / 4 ? 0x00EE3333 : 0x00FF8800;
    for (int py = bar_y; py < bar_y + bar_h; py++) {
        for (int px = bar_x; px < bar_x + bar_w; px++) {
            vg_draw_pixel(px, py, px < bar_x + fill_w ? fill_color : 0x00333333);
        }
    }

    // wave counter below the bar
    char wave_str[16];
    snprintf(wave_str, sizeof(wave_str), "wave %d", wave_number);
    font_draw_string(10, 30, wave_str, 0x00FFFFFF);

    // gold counter (top-right, unchanged)
    char gold_str[32];
    snprintf(gold_str, sizeof(gold_str), "gold %d", player_gold);
    uint16_t tw = font_string_width(gold_str);
    font_draw_string((uint16_t)(vg_get_x_res() - tw - 10), 10, gold_str, 0x00FFD700);
}

bool game_is_over(void) {
    return game_over;
}

#define GO_ITEM_GAP  32   // 16px text + 16px spacing, same rhythm as the menu
#define GO_OPTS_Y    0    // options start at screen centre (added to cy at runtime)
#define GO_DOT_SIZE  6
#define GO_DOT_PAD   14   // px between dot right edge and text left edge

#define GO_COLOR_SELECTED 0x00E94560
#define GO_COLOR_NORMAL   0x00B0B0C0

void game_draw_gameover(void) {
    uint16_t sw = (uint16_t) vg_get_x_res();
    int cx = (int) sw / 2;
    int cy = (int) vg_get_y_res() / 2;

    // "game over" at 3x scale with drop shadow
    const char *go_str = "game over";
    uint16_t go_w = font_string_width_scaled(go_str, 3);
    uint16_t go_x = (uint16_t)(cx - go_w / 2);
    uint16_t go_y = (uint16_t)(cy - 100);
    font_draw_string_scaled(go_x + 3, go_y + 3, go_str, 0x00660000, 3);
    font_draw_string_scaled(go_x,     go_y,     go_str, 0x00FF3333, 3);

    // vertical option list — same style as the main menu
    for (int i = 0; i < 2; i++) {
        uint16_t iw    = font_string_width(go_labels[i]);
        uint16_t ix    = (uint16_t)((sw - iw) / 2);
        uint16_t iy    = (uint16_t)(cy + i * GO_ITEM_GAP);
        uint32_t color = (i == go_selected) ? GO_COLOR_SELECTED : GO_COLOR_NORMAL;

        if (i == go_selected) {
            uint16_t dot_x = ix - GO_DOT_PAD - GO_DOT_SIZE;
            uint16_t dot_y = (uint16_t)(iy + 8 - GO_DOT_SIZE / 2);
            for (int dy = 0; dy < GO_DOT_SIZE; dy++)
                for (int dx = 0; dx < GO_DOT_SIZE; dx++)
                    vg_draw_pixel(dot_x + dx, dot_y + dy, GO_COLOR_SELECTED);
        }

        font_draw_string(ix, iy, go_labels[i], color);
    }
}

int game_gameover_click(void) {
    int cy = (int) vg_get_y_res() / 2;
    for (int i = 0; i < 2; i++) {
        int iy = cy + i * GO_ITEM_GAP;
        if (cursor_y >= iy - 4 && cursor_y < iy + 20)
            return i + 1; // 1=retry, 2=menu
    }
    return 0;
}

bool game_gameover_handle_key(uint8_t sc) {
    switch (sc) {
        case KEY_UP_MAKE:
        case KEY_W_MAKE:
            if (go_selected > 0) go_selected--;
            break;
        case KEY_DOWN_MAKE:
        case KEY_S_MAKE:
            if (go_selected < 1) go_selected++;
            break;
        case KEY_ENTER_MAKE:
            return true;
        default:
            break;
    }
    return false;
}

int game_gameover_get_selected(void) {
    return go_selected;
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
