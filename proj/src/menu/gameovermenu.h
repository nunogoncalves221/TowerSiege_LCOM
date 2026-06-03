#ifndef GAMEOVERMENU_H
#define GAMEOVERMENU_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    GAMEOVER_RETRY = 0,
    GAMEOVER_MENU,
    GAMEOVER_OPTION_COUNT
} gameover_option_t;

void gameover_reset(void);
gameover_option_t gameover_get_selected(void);
bool gameover_handle_key(uint8_t scancode);
void gameover_draw(int score);
int  gameover_click(int16_t cursor_y);

#endif
