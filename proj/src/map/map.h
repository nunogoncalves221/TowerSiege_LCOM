/**
 * @file map.h
 * @brief Tile-based game map definition and rendering.
 *
 * The map is a fixed grid of ::MAP_COLS × ::MAP_ROWS tiles. Each tile is
 * either grass (placeable), path (enemy route), or blank (tower slot).
 * The map caches its rendered state and only redraws when marked dirty.
 */

#ifndef MAP_H
#define MAP_H

#include <lcom/lcf.h>
#include "difficulty.h"

#define MAP_COLS  12
#define MAP_ROWS  9
#define TILE_SIZE 96

/** @brief Type of a single map tile. */
typedef enum {
    TILE_GRASS, /**< Open ground */
    TILE_PATH,  /**< Enemy walking route. */
    TILE_BLANK  /**< Towers can be placed here. */
} tile_t;

/**
 * @brief Loads the map layout for the given difficulty and prepares tile sprites.
 *
 * @param diff Selected difficulty, which may determine the map variant used.
 */
void map_init(difficulty_t diff);

/**
 * @brief Frees all resources allocated by map_init().
 */
void map_destroy(void);

/**
 * @brief Draws the map to the screen if the tile cache is dirty.
 *
 * @return true if the tiles were redrawn, false if the cached frame was reused.
 */
bool map_draw(void);

/**
 * @brief Copies the current rendered map into the tile cache.
 *
 * Call after a full redraw so subsequent frames can skip the redraw.
 */
void map_update_cache(void);

/**
 * @brief Marks the tile cache as stale, forcing a full redraw on the next map_draw().
 */
void map_mark_dirty(void);

/**
 * @brief Returns the tile type at the given grid coordinates.
 *
 * @param col Column index (0-based, 0 = leftmost).
 * @param row Row index (0-based, 0 = topmost).
 * @return The ::tile_t at that position.
 */
tile_t map_get_tile(int col, int row);

#endif /* MAP_H */
