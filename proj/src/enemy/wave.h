#ifndef WAVE_H
#define WAVE_H

#include <lcom/lcf.h>
#include "enemy.h"

void wave_init(void); // initializes wave state
void wave_tick(enemy_t **enemies, int *enemy_count); // spawns enemies
int wave_get_number(void);

#endif
