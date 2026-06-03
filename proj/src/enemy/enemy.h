#ifndef ENEMY_H
#define ENEMY_H

#include <lcom/lcf.h>
#include "../sprite/sprite.h"

// direction enumeration for grid movement
typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} direction_t;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t speed;
    int16_t hp;
    int gold_value; // gold awarded to player on kill
    sprite_t *sp;
    bool active; // false if dead or reached the end
    direction_t dir; // track which way the enemy is currently facing
} enemy_t;

// allocates and initializes a single enemy at the start coordinates
enemy_t* enemy_create(xpm_map_t xpm, int16_t start_x, int16_t start_y, int16_t hp, int16_t speed);

// frees enemy memory
void enemy_destroy(enemy_t *e);

// calculates the next step over the TILE_PATH grid
void enemy_update(enemy_t *e);

// draws the enemy on the screen
void enemy_draw(enemy_t *e);

#endif
