/**
 * @file wave.h
 * @brief Enemy wave spawning logic.
 *
 * Manages the current wave number and the timed spawning of enemies
 * into the active enemy array. The wave advances automatically once
 * all enemies from the previous wave have been cleared.
 */

#ifndef WAVE_H
#define WAVE_H

#include <lcom/lcf.h>
#include "enemy.h"

/**
 * @brief Resets wave state to the beginning of wave 1.
 *
 * Should be called once during game initialisation or on game reset.
 */
void wave_init(void);

/**
 * @brief Advances the wave timer and spawns enemies when appropriate.
 *
 * Must be called once per game tick. When the spawn timer fires,
 * a new ::enemy_t is appended to @p enemies and @p enemy_count is
 * incremented. The function handles wave transitions internally.
 *
 * @param enemies      Pointer to the array of active enemy pointers.
 * @param enemy_count  Pointer to the current number of active enemies;
 *                     incremented each time an enemy is spawned.
 */
void wave_tick(enemy_t **enemies, int *enemy_count);

/**
 * @brief Returns the current wave number (1-based).
 *
 * @return The wave that is currently being played.
 */
int wave_get_number(void);

#endif /* WAVE_H */
