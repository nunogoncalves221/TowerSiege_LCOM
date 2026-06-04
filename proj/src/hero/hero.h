#ifndef _HERO_H_
#define _HERO_H_

#include <stdbool.h>
#include "enemy.h"

// sets up the hero's initial position, health and animations
void hero_init(void);

// cleans up memory when the game is reset or closed
void hero_destroy(void);

// passes the current keyboard state from the main game loop to the hero
void hero_update_input(bool w, bool a, bool s, bool d, bool space);

// handles movement, attack state machine, and collisions with enemies
void hero_tick(enemy_t **enemies, int enemy_count);

// draws the hero sprite and his floating HP bar
void hero_draw(void);

#endif
