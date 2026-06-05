/**
 * @file enemy_manager.h
 * @brief Batch update and draw for all active enemies.
 *
 * Iterates over the active enemy array each tick, delegating per-enemy
 * movement to enemy_update(), detecting base-reach events, and culling
 * dead enemies from the array.
 */

#ifndef _ENEMY_MANAGER_H_
#define _ENEMY_MANAGER_H_

#include <stdbool.h>
#include "enemy.h"

/**
 * @brief Updates all active enemies and handles collisions and deaths.
 *
 * Must be called once per game tick. Moves each enemy, checks whether it
 * has reached the player's base (removing a life), and removes enemies
 * whose hp has dropped to zero.
 *
 * @param enemies      Pointer to the array of active enemy pointers.
 * @param enemy_count  Pointer to the current number of active enemies;
 *                     decremented when an enemy is removed.
 * @return true if the player's base lost its last life this tick.
 */
bool enemy_manager_tick(enemy_t **enemies, int *enemy_count);

/**
 * @brief Draws all active enemies.
 *
 * @param enemies     Array of active enemy pointers.
 * @param enemy_count Number of active enemies in the array.
 */
void enemy_manager_draw(enemy_t **enemies, int enemy_count);

#endif /* _ENEMY_MANAGER_H_ */
