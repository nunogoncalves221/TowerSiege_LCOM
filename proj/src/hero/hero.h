/**
 * @file hero.h
 * @brief Player-controlled hero character.
 *
 * Manages the hero's position, health, animations, and melee attack state
 * machine. The game loop drives the hero by forwarding keyboard state each
 * tick and calling hero_tick() to resolve movement and combat.
 */

#ifndef _HERO_H_
#define _HERO_H_

#include <stdbool.h>
#include "enemy.h"

/**
 * @brief Sets up the hero's initial position, health, and animations.
 *
 * Must be called once during game initialisation before any other hero function.
 */
void hero_init(void);

/**
 * @brief Frees all memory owned by the hero.
 *
 * Call when the game is reset or the game state is exited.
 */
void hero_destroy(void);

/**
 * @brief Passes the current keyboard state to the hero for the next tick.
 *
 * @param w true while the W key is held (move up).
 * @param a true while the A key is held (move left).
 * @param s true while the S key is held (move down).
 * @param d true while the D key is held (move right).
 * @param f true while the Space key is held (attack).
 */
void hero_update_input(bool w, bool a, bool s, bool d, bool f);

/**
 * @brief Advances the hero simulation by one tick.
 *
 * Applies movement, advances the attack state machine, and checks melee
 * collisions against the active enemies.
 *
 * @param enemies     Array of active enemy pointers.
 * @param enemy_count Number of active enemies in the array.
 */
void hero_tick(enemy_t **enemies, int enemy_count);

/**
 * @brief Draws the hero sprite and his floating HP bar.
 */
void hero_draw(void);

#endif /* _HERO_H_ */
