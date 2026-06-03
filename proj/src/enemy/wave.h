#ifndef WAVE_H
#define WAVE_H

#include <lcom/lcf.h>
#include "enemy.h"

// initializes wave state and computes enemy spawn position from sprite dimensions
void wave_init(void);

// spawns enemies and advances the wave counter; call once per game tick
void wave_tick(enemy_t **enemies, int *enemy_count);

int wave_get_number(void);

#endif
