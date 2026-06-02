#ifndef GAME_H
#define GAME_H

#include <lcom/lcf.h>

#define TIMER_FREQ 60
#define FPS 30
#define HERO_SPEED 4  // pixels per frame

// WASD Set-1 scancodes (make) — break = make | 0x80
#define KEY_W_MAKE     0x11
#define KEY_A_MAKE     0x1E
#define KEY_S_MAKE     0x1F
#define KEY_D_MAKE     0x20
#define KEY_W_BREAK    0x91
#define KEY_A_BREAK    0x9E
#define KEY_S_BREAK    0x9F
#define KEY_D_BREAK    0xA0
#define KEY_UP_MAKE    0x48
#define KEY_DOWN_MAKE  0x50
#define KEY_ENTER_MAKE 0x1C

void game_reset(void);
void game_tick(void);
void game_draw(void);
void game_handle_key(uint8_t scancode);
void game_update_cursor(int16_t dx, int16_t dy);
void game_handle_click(void);
bool game_is_over(void);
void game_draw_gameover(void);
int  game_gameover_click(void);              // 0=none, 1=retry, 2=menu (mouse)
bool game_gameover_handle_key(uint8_t sc);  // returns true when Enter pressed
int  game_gameover_get_selected(void);       // 0=retry, 1=menu

#endif
