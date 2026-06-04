#ifndef GAMEOVERMENU_HS_H
#define GAMEOVERMENU_HS_H

#include <stdint.h>
#include <stdbool.h>

void gameover_hs_reset(void);
bool gameover_hs_handle_key(uint8_t scancode); // returns true when name is confirmed
void gameover_hs_draw(int score);

#endif
