/**
 * @file bullet.h
 * @brief Projectile system for tower attacks.
 *
 * Manages a fixed pool of in-flight bullets. Each bullet tracks a specific
 * enemy target and travels toward it each tick. On impact the target takes
 * damage and the bullet is returned to the pool.
 */

#ifndef BULLET_H
#define BULLET_H

#include <lcom/lcf.h>
#include "sprite.h"
#include "enemy.h"

/**
 * @brief Initialises the bullet pool.
 *
 * Must be called once before any other bullet function.
 */
void bullet_init(void);

/**
 * @brief Spawns a new bullet aimed at the given enemy.
 *
 * If the pool is exhausted, the fire request is silently dropped.
 *
 * @param from_x Origin X position in screen pixels (tower centre).
 * @param from_y Origin Y position in screen pixels (tower centre).
 * @param target Pointer to the enemy this bullet will track.
 * @param damage Hit points to remove from the target on impact.
 * @param sp     Sprite used to render the bullet.
 */
void bullet_fire(int16_t from_x, int16_t from_y, enemy_t *target, int damage, sprite_t *sp);

/**
 * @brief Removes all references to a destroyed enemy from the bullet pool.
 *
 * Must be called whenever an enemy is removed so that in-flight bullets
 * targeting it do not dereference a dangling pointer.
 *
 * @param e Enemy that has just been removed.
 */
void bullet_invalidate_target(enemy_t *e);

/**
 * @brief Advances all in-flight bullets by one tick.
 *
 * Moves each bullet toward its target and applies damage on impact.
 */
void bullet_tick(void);

/**
 * @brief Draws all in-flight bullets.
 */
void bullet_draw(void);

#endif /* BULLET_H */
