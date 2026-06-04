#ifndef _ENEMY_MANAGER_H_
#define _ENEMY_MANAGER_H_

#include <stdbool.h>
#include "enemy.h"

// updates all enemies, handles screen bounds, player damage, and removes dead ones
// returns true if the player's base lost its last life this tick
bool enemy_manager_tick(enemy_t **enemies, int *enemy_count);

// draws all active enemies safely
void enemy_manager_draw(enemy_t **enemies, int enemy_count);

#endif
