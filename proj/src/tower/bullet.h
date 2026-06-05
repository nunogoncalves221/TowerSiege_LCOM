#ifndef BULLET_H
#define BULLET_H

#include <lcom/lcf.h>
#include "sprite.h"
#include "enemy.h"

void bullet_init(void);
void bullet_fire(int16_t from_x, int16_t from_y, enemy_t *target, int damage, sprite_t *sp);
void bullet_invalidate_target(enemy_t *e);
void bullet_tick(void);
void bullet_draw(void);

#endif
