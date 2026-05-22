#ifndef GAME_H
#define GAME_H

#include <lcom/lcf.h>

#define TIMER_FREQ 60
#define FPS 60
#define HERO_SPEED 4  // pixels per frame

// WASD Set-1 scancodes (make) — break = make | 0x80
#define KEY_W_MAKE  0x11
#define KEY_A_MAKE  0x1E
#define KEY_S_MAKE  0x1F
#define KEY_D_MAKE  0x20
#define KEY_W_BREAK 0x91
#define KEY_A_BREAK 0x9E
#define KEY_S_BREAK 0x9F
#define KEY_D_BREAK 0xA0

void game_reset(void);
void game_tick(void);
void game_draw(void);
void game_handle_key(uint8_t scancode);

#endif
