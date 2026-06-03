#ifndef GAME_H
#define GAME_H

#include <lcom/lcf.h>

// --- constants ---

// core game mechanics
#define TIMER_FREQ 60
#define FPS 30
#define MAX_LIVES 10   // starting lives for the player's base
#define MAX_ENEMIES 20 // maximum concurrent enemies on screen

// hero attributes
#define HERO_SPEED 4  // movement speed in pixels per frame
#define HERO_BASE_X 750 // hero's starting x position
#define HERO_BASE_Y 550 // hero's starting y position

// wave mechanics
#define WAVE_DELAY 300 // ticks to wait between waves (5 seconds at 60hz)

// --- keyboard scancodes ---
// based on set 1 make codes; break codes are make | 0x80

// movement keys (wasd)
#define KEY_W_MAKE     0x11
#define KEY_A_MAKE     0x1E
#define KEY_S_MAKE     0x1F
#define KEY_D_MAKE     0x20
#define KEY_W_BREAK    0x91
#define KEY_A_BREAK    0x9E
#define KEY_S_BREAK    0x9F
#define KEY_D_BREAK    0xA0

// menu navigation keys
#define KEY_UP_MAKE    0x48
#define KEY_DOWN_MAKE  0x50
#define KEY_ENTER_MAKE 0x1C

// action keys
#define KEY_F_MAKE 0x21     // 'f' key for hero attack
#define KEY_F_BREAK 0xA1

// --- function prototypes ---

// main game loop
void game_reset(void);
void game_tick(void);
void game_draw(void);
bool game_is_over(void);

// player input
void game_handle_key(uint8_t scancode);
void game_update_cursor(int16_t dx, int16_t dy);
void game_handle_click(void);

// score and cursor accessors (used by gameovermenu)
int    game_get_score(void);
int16_t game_get_cursor_y(void);

// gold
int get_player_gold();
void add_player_gold(int amount);

// ui function for hero module
void draw_hp_bar(int x, int y, int width, int height, int current, int max, uint32_t color_good, uint32_t color_bg, uint32_t color_border);

#endif
