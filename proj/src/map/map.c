#include "map.h"
#include "sprite.h"
#include "video.h"
#include "../assets/grass.xpm"
#include "../assets/path.xpm"
#include "../assets/blank.xpm"

static const tile_t map_grid[MAP_ROWS][MAP_COLS] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 1, 1, 0, 0, 0, 2, 0, 2, 0, 0, 0 },
    { 0, 2, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 2, 0 },
    { 0, 2, 1, 0, 0, 1, 2, 0, 0, 1, 0, 0 },
    { 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 2, 0 },
    { 0, 2, 1, 0, 0, 1, 2, 0, 0, 1, 0, 0 },
    { 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1 },
    { 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0 },
};

static sprite_t *grass_sprite = NULL;
static sprite_t *path_sprite  = NULL;
static sprite_t *blank_sprite = NULL;

void map_init(void) {
    if (grass_sprite == NULL)
        grass_sprite = create_sprite((xpm_map_t) grass);
    if (path_sprite == NULL)
        path_sprite = create_sprite((xpm_map_t) path);
    if (blank_sprite == NULL)
        blank_sprite = create_sprite((xpm_map_t) blank);
}

void map_draw(void) {
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            int16_t x = (int16_t)(col * TILE_SIZE);
            int16_t y = (int16_t)(row * TILE_SIZE);
            switch (map_grid[row][col]) {
                case TILE_PATH:  draw_sprite(path_sprite,  x, y); break;
                case TILE_BLANK: draw_sprite(blank_sprite, x, y); break;
                default:         draw_sprite(grass_sprite, x, y); break;
            }
        }
    }
}

tile_t map_get_tile(int col, int row) {
    if (col < 0 || col >= MAP_COLS || row < 0 || row >= MAP_ROWS)
        return TILE_GRASS;
    return map_grid[row][col];
}
