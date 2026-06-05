#include "map.h"
#include "sprite.h"
#include "video.h"
#include "../assets/grass.xpm"
#include "../assets/path.xpm"
#include "../assets/blank.xpm"
#include "../assets/archer_1.xpm"
#include "../assets/mage_1.xpm"

static const tile_t map_easy[MAP_ROWS][MAP_COLS] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 1, 1, 0, 0, 0, 2, 0, 2, 0, 0, 0 },
    { 0, 2, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 2, 0 },
    { 0, 2, 1, 0, 0, 1, 2, 0, 0, 1, 0, 0 },
    { 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 2, 0 },
    { 0, 2, 1, 1, 1, 1, 2, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 2, 0, 0, 0, 0, 0, 1, 1, 1 },
    { 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2, 0 },
};

static const tile_t map_medium[MAP_ROWS][MAP_COLS] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 2, 1, 0, 0, 2, 0, 0, 2, 0, 0, 0 },
    { 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0 },
    { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0 },
    { 0, 2, 1, 1, 1, 1, 2, 0, 1, 2, 0, 0 },
    { 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 },
    { 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2, 0 },
};

static const tile_t map_hard[MAP_ROWS][MAP_COLS] = {
    { 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0 },
    { 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 2, 0, 0, 1, 2, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0 },
    { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 },
    { 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0 },
};

static const tile_t (*active_map)[MAP_COLS] = map_easy;

static sprite_t *grass_sprite  = NULL;
static sprite_t *path_sprite   = NULL;
static sprite_t *blank_sprite  = NULL;
static sprite_t *archer_sprite = NULL;
static sprite_t *mage_sprite   = NULL;

static uint8_t *map_cache = NULL;
static bool map_dirty = true;

#define MAP_CACHE_W (MAP_COLS * TILE_SIZE)
#define MAP_CACHE_H (MAP_ROWS * TILE_SIZE)

void map_init(difficulty_t diff) {
    if (diff == DIFF_MEDIUM)      active_map = map_medium;
    else if (diff == DIFF_HARD)   active_map = map_hard;
    else                          active_map = map_easy;
    if (grass_sprite == NULL)
        grass_sprite = create_sprite((xpm_map_t) grass);
    if (path_sprite == NULL)
        path_sprite = create_sprite((xpm_map_t) path);
    if (blank_sprite == NULL)
        blank_sprite = create_sprite((xpm_map_t) blank);
    if (archer_sprite == NULL)
        archer_sprite = create_sprite((xpm_map_t) archer_1);
    if (mage_sprite == NULL)
        mage_sprite = create_sprite((xpm_map_t) mage_1);

    if (map_cache == NULL)
        map_cache = malloc((uint32_t)MAP_CACHE_W * MAP_CACHE_H * get_bytes_per_pixel());

    map_dirty = true;
}

void map_mark_dirty(void) {
    map_dirty = true;
}

bool map_draw(void) {
    if (!map_dirty) {
        vg_blit_region(map_cache, MAP_CACHE_W, 0, 0, MAP_CACHE_W, MAP_CACHE_H);
        return false;
    }

    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            int16_t x = (int16_t)(col * TILE_SIZE);
            int16_t y = (int16_t)(row * TILE_SIZE);
            switch (active_map[row][col]) {
                case TILE_PATH:  draw_sprite(path_sprite,  x, y); break;
                case TILE_BLANK: draw_sprite(blank_sprite, x, y); break;
                default:         draw_sprite(grass_sprite, x, y); break;
            }
        }
    }

    if (archer_sprite != NULL)
        draw_sprite(archer_sprite, 5 * TILE_SIZE, 8 * TILE_SIZE);
    if (mage_sprite != NULL)
        draw_sprite(mage_sprite, 6 * TILE_SIZE, 8 * TILE_SIZE);

    map_dirty = false;
    return true;
}

void map_update_cache(void) {
    vg_copy_region(0, 0, MAP_CACHE_W, MAP_CACHE_H, map_cache, MAP_CACHE_W);
}

tile_t map_get_tile(int col, int row) {
    if (col < 0 || col >= MAP_COLS || row < 0 || row >= MAP_ROWS)
        return TILE_GRASS;
    return active_map[row][col];
}
