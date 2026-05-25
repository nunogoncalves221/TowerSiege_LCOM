#include "enemy.h"
#include "map.h"
#include "video.h"  // needed for vg_get_x_res()
#include <stdlib.h>

enemy_t* enemy_create(xpm_map_t xpm, int16_t start_x, int16_t start_y, int16_t hp, int16_t speed) {
    enemy_t *e = (enemy_t *) malloc(sizeof(enemy_t));
    if (e == NULL) return NULL;

    e->sp = create_sprite(xpm);
    if (e->sp == NULL) {
        free(e);
        return NULL;
    }

    e->x = start_x;
    e->y = start_y;
    e->hp = hp;
    e->speed = speed;
    e->active = true;

    return e;
}

void enemy_destroy(enemy_t *e) {
    if (e == NULL) return;
    if (e->sp != NULL) destroy_sprite(e->sp);
    free(e);
}

void enemy_update(enemy_t *e) {
    if (e == NULL || !e->active) return;

    // convert pixel position to map grid coordinates (center of the sprite)
    //int current_col = (e->x + e->sp->width / 2) / TILE_SIZE;
    //int current_row = (e->y + e->sp->height / 2) / TILE_SIZE;

    // simple horizontal move for testing
    e->x += e->speed; 

    // out of bounds safety check
    if (e->x > 1152) {
        e->active = false; // reached the castle
    }
}

void enemy_draw(enemy_t *e) {
    if (e == NULL || !e->active || e->sp == NULL) return;
    
    // cast to uint16_t to comply with the team's warning fix
    draw_sprite(e->sp, e->x, e->y);
}
