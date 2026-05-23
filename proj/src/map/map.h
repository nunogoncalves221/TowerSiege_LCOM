#ifndef MAP_H
#define MAP_H

#include <lcom/lcf.h>

#define MAP_COLS 12
#define MAP_ROWS 9
#define TILE_SIZE 96

typedef enum {
    TILE_GRASS,
    TILE_PATH,
    TILE_BLANK
} tile_t;

void map_init(void);
void map_draw(void);

tile_t map_get_tile(int col, int row);

#endif
