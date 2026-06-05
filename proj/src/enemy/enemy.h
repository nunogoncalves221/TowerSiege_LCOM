/**
 * @file enemy.h
 * @brief Enemy entity definition and per-enemy lifecycle functions.
 *
 * Declares the ::enemy_t structure that represents a single enemy on the map,
 * along with functions to create, update, draw, and destroy one enemy at a
 * time. Higher-level coordination of multiple enemies is handled by the
 * enemy_manager module.
 */

#ifndef ENEMY_H
#define ENEMY_H

#include <lcom/lcf.h>
#include "../sprite/sprite.h"

/** @brief Movement direction used for grid-based path following. */
typedef enum {
    DIR_UP,    /**< Enemy is moving upward. */
    DIR_DOWN,  /**< Enemy is moving downward. */
    DIR_LEFT,  /**< Enemy is moving to the left. */
    DIR_RIGHT  /**< Enemy is moving to the right. */
} direction_t;

/** @brief Represents a single active enemy on the map. */
typedef struct {
    int16_t     x;          /**< Current horizontal position in screen pixels. */
    int16_t     y;          /**< Current vertical position in screen pixels. */
    int16_t     speed;      /**< Movement speed in pixels per game tick. */
    int16_t     hp;         /**< Remaining hit points. */
    int         gold_value; /**< Gold awarded to the player upon killing this enemy. */
    sprite_t   *sp;         /**< Visual representation of the enemy. */
    bool        active;     /**< false when dead or after reaching the base. */
    direction_t dir;        /**< Direction the enemy is currently travelling. */
} enemy_t;

/**
 * @brief Allocates and initialises a single enemy at the given coordinates.
 *
 * @param xpm     XPM image used as the enemy's sprite.
 * @param start_x Initial horizontal position in screen pixels.
 * @param start_y Initial vertical position in screen pixels.
 * @param hp      Starting hit points.
 * @param speed   Movement speed in pixels per game tick.
 * @return Pointer to the newly allocated ::enemy_t, or NULL on failure.
 */
enemy_t *enemy_create(xpm_map_t xpm, int16_t start_x, int16_t start_y, int16_t hp, int16_t speed);

/**
 * @brief Frees all memory owned by an enemy.
 *
 * @param e Enemy to destroy.
 */
void enemy_destroy(enemy_t *e);

/**
 * @brief Advances an enemy one step along the TILE_PATH grid.
 *
 * Must be called once per game tick for each active enemy.
 *
 * @param e Enemy to update.
 */
void enemy_update(enemy_t *e);

/**
 * @brief Draws the enemy sprite at its current position.
 *
 * @param e Enemy to draw.
 */
void enemy_draw(enemy_t *e);

#endif /* ENEMY_H */
