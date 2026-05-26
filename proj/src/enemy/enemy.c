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
    e->dir = DIR_RIGHT; // all enemies start marching east 

    return e;
}

void enemy_destroy(enemy_t *e) {
    if (e == NULL) return;
    if (e->sp != NULL) destroy_sprite(e->sp);
    free(e);
}

void enemy_update(enemy_t *e) {
    if (e == NULL || !e->active || e->sp == NULL) return;

    // calculate the exact center point of the enemy sprite
    int16_t center_x = e->x + (int16_t)(e->sp->width / 2);
    int16_t center_y = e->y + (int16_t)(e->sp->height / 2);

    // get the current map cell (column and row) based on the center
    int col = center_x / TILE_SIZE;
    int row = center_y / TILE_SIZE;

    // grid-snapping logic at the tile center to search for turns
    // we check if the center point is within a small threshold of the tile center
    int16_t tile_center_x = (int16_t)(col * TILE_SIZE + TILE_SIZE / 2);
    int16_t tile_center_y = (int16_t)(row * TILE_SIZE + TILE_SIZE / 2);

    // 3. check decision point depending ONLY on the current axis of movement
    bool at_decision_point = false;

    if ((e->dir == DIR_RIGHT || e->dir == DIR_LEFT) && abs(center_x - tile_center_x) <= e->speed) {
        at_decision_point = true;
    }
    if ((e->dir == DIR_UP || e->dir == DIR_DOWN) && abs(center_y - tile_center_y) <= e->speed) {
        at_decision_point = true;
    }

    if (at_decision_point) {
        // look ahead based on current direction to see if TILE_PATH continues
        bool path_ahead = false;
        
        if (e->dir == DIR_RIGHT && col + 1 < MAP_COLS && map_get_tile(col + 1, row) == TILE_PATH) path_ahead = true;
        else if (e->dir == DIR_LEFT && col - 1 >= 0 && map_get_tile(col - 1, row) == TILE_PATH) path_ahead = true;
        else if (e->dir == DIR_UP && row - 1 >= 0 && map_get_tile(col, row - 1) == TILE_PATH) path_ahead = true;
        else if (e->dir == DIR_DOWN && row + 1 < MAP_ROWS && map_get_tile(col, row + 1) == TILE_PATH) path_ahead = true;

        // if the path ends or turns, evaluate neighbors to find the sand road
        if (!path_ahead) {
            // snap to center before turning to lock into the new axis grid
            e->x = tile_center_x - (int16_t)(e->sp->width / 2);
            e->y = tile_center_y - (int16_t)(e->sp->height / 2);

            if (e->dir != DIR_LEFT && col + 1 < MAP_COLS && map_get_tile(col + 1, row) == TILE_PATH) {
                e->dir = DIR_RIGHT;
            }
            else if (e->dir != DIR_RIGHT && col - 1 >= 0 && map_get_tile(col - 1, row) == TILE_PATH) {
                e->dir = DIR_LEFT;
            }
            else if (e->dir != DIR_UP && row + 1 < MAP_ROWS && map_get_tile(col, row + 1) == TILE_PATH) {
                e->dir = DIR_DOWN;
            }
            else if (e->dir != DIR_DOWN && row - 1 >= 0 && map_get_tile(col, row - 1) == TILE_PATH) {
                e->dir = DIR_UP;
            }
        }
    }

    // execute the movement according to the verified direction
    switch (e->dir) {
        case DIR_RIGHT: e->x += e->speed; break;
        case DIR_LEFT:  e->x -= e->speed; break;
        case DIR_UP:    e->y -= e->speed; break;
        case DIR_DOWN:  e->y += e->speed; break;
    }

    // dynamic resolution limit -> check if the enemy exited the right-hand boundary of the final path tile
    if (e->x > (int16_t) vg_get_x_res() || e->y > (int16_t) vg_get_y_res() || e->y < -100) {
        e->active = false; 
    }
}

void enemy_draw(enemy_t *e) {
    if (e == NULL || !e->active || e->sp == NULL) return;
    
    // cast to uint16_t to comply with the team's warning fix
    draw_sprite(e->sp, e->x, e->y);
}
