/**
 * @file tower.h
 * @brief Tower placement, upgrading, and attack logic.
 *
 * Manages all placed towers on the map. Each tower scans for enemies in
 * range every tick and fires a bullet at the closest one. A click-driven
 * popup lets the player remove or upgrade towers on blank tiles.
 */

#ifndef TOWER_H
#define TOWER_H

#include <lcom/lcf.h>
#include "sprite.h"
#include "map.h"
#include "enemy.h"
#include "difficulty.h"

/** @brief Type of tower occupying a grid slot. */
typedef enum {
    TOWER_NONE,   /**< Slot is empty (no tower placed). */
    TOWER_ARCHER, /**< Archer tower */
    TOWER_MAGE    /**< Mage tower */
} tower_type_t;

#define TOWER_ARCHER_COST     200
#define TOWER_MAGE_COST       250
#define TOWER_ARCHER_UPG_COST 250
#define TOWER_MAGE_UPG_COST   300

#define TOWER_ARCHER_BASE_DMG      10
#define TOWER_ARCHER_BASE_RANGE    200
#define TOWER_ARCHER_BASE_COOLDOWN 55

#define TOWER_MAGE_BASE_DMG      40
#define TOWER_MAGE_BASE_RANGE    160
#define TOWER_MAGE_BASE_COOLDOWN 60

#define TOWER_ARCHER_UPG_DMG_INC   10
#define TOWER_ARCHER_UPG_RANGE_INC 50
#define TOWER_ARCHER_UPG_CD_DEC    8

#define TOWER_MAGE_UPG_DMG_INC   35
#define TOWER_MAGE_UPG_RANGE_INC 40

/** @brief Represents a single placed tower and all its combat stats. */
typedef struct {
    tower_type_t type;           /**< Which kind of tower this is. */
    int          level;          /**< Current upgrade level (1 = base, 2 = upgraded). */
    sprite_t    *sp;             /**< Sprite used to render the tower. */
    int          col, row;       /**< Grid coordinates of the tile the tower occupies. */
    int          range;          /**< Attack range radius in pixels. */
    uint32_t     range_sq;       /**< Squared range, used for fast distance checks. */
    int          damage;         /**< Damage dealt per bullet. */
    int          attack_cooldown;/**< Ticks between consecutive shots. */
    int          attack_timer;   /**< Ticks remaining until the next shot. */
} tower_t;

/** @brief Associates a grid position with the tower placed on it. */
typedef struct {
    int      col, row; /**< Grid coordinates of this slot. */
    tower_t *tower;    /**< Pointer to the placed tower, or NULL if empty. */
} tower_slot_t;

/**
 * @brief Initialises the tower grid and loads tower sprites.
 *
 * Must be called once after map_init() before any other tower function.
 *
 * @param diff Selected difficulty, used to scale tower costs if applicable.
 */
void tower_init(difficulty_t diff);

/**
 * @brief Handles a left-click at the given pixel coordinates.
 *
 * If the click lands on a blank tile that has no tower, opens the placement
 * popup. If a tower is already there, opens the upgrade popup instead.
 *
 * @param px Click X position in screen pixels.
 * @param py Click Y position in screen pixels.
 */
void tower_handle_click_px(int px, int py);

/**
 * @brief Advances all placed towers by one tick.
 *
 * Each tower scans for enemies within its range and fires a bullet at the
 * closest one when its cooldown has expired.
 *
 * @param enemies     Array of active enemy pointers.
 * @param enemy_count Number of active enemies in the array.
 */
void tower_tick(enemy_t **enemies, int enemy_count);

/**
 * @brief Draws all placed towers on the map.
 */
void tower_draw(void);

/**
 * @brief Returns whether the tower placement/upgrade popup is currently open.
 *
 * @return true if the popup is visible and consuming input.
 */
bool tower_popup_is_open(void);

/**
 * @brief Forwards a keyboard scancode to the tower popup.
 *
 * @param scancode Raw keyboard scancode (set 1 make code).
 * @return true if the key was consumed by the popup (caller should not
 *         forward it to other handlers).
 */
bool tower_popup_handle_key(uint8_t scancode);

/**
 * @brief Draws the tower placement/upgrade popup overlay.
 */
void tower_popup_draw(void);

/**
 * @brief Closes the tower popup without taking any action.
 */
void tower_popup_close(void);

#endif /* TOWER_H */
