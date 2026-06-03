#ifndef TOWER_H
#define TOWER_H

#include <lcom/lcf.h>
#include "sprite.h"
#include "map.h"
#include "enemy.h"

// --- types ---

typedef enum { TOWER_NONE, TOWER_ARCHER, TOWER_MAGE } tower_type_t;
typedef enum { UI_NONE, UI_BUILD, UI_UPGRADE } ui_state_t;

// --- constants ---

// tower costs
#define TOWER_ARCHER_COST 150
#define TOWER_MAGE_COST   300
#define TOWER_ARCHER_UPG_COST 100
#define TOWER_MAGE_UPG_COST   250

// archer tower stats (level 1) — base dmg/cd unchanged per user
#define TOWER_ARCHER_BASE_DMG       10
#define TOWER_ARCHER_BASE_RANGE     200
#define TOWER_ARCHER_BASE_COOLDOWN  55

// mage tower stats (level 1) — significantly stronger, faster
#define TOWER_MAGE_BASE_DMG         40
#define TOWER_MAGE_BASE_RANGE       160
#define TOWER_MAGE_BASE_COOLDOWN    60

// archer tower upgrade (level 2)
#define TOWER_ARCHER_UPG_DMG_INC    10
#define TOWER_ARCHER_UPG_RANGE_INC  50
#define TOWER_ARCHER_UPG_CD_DEC     8

// mage tower upgrade (level 2)
#define TOWER_MAGE_UPG_DMG_INC      35
#define TOWER_MAGE_UPG_RANGE_INC    40

// --- data structures ---

// represents a single tower instance on the map
typedef struct {
    tower_type_t type;
    int level;           // level of the tower (1 or 2)
    sprite_t *sp;        // sprite for the current level
    int col, row;        // grid position (column, row)
    int range;           // attack range in pixels
    uint32_t range_sq;   // cached squared range to save CPU cycles
    int damage;          // damage per shot
    int attack_cooldown; // frames between shots (cooldown)
    int attack_timer;    // countdown timer for the next shot
} tower_t;

// represents a grid location where a tower can be built
typedef struct {
    int col, row;   // grid position (column, row)
    tower_t *tower; // pointer to the tower in this slot, or null if empty
} tower_slot_t;

// --- function prototypes ---

void tower_init(void);
void tower_handle_click(int col, int row);           // handles tower logic based on player clicks
void tower_tick(enemy_t **enemies, int enemy_count); // updates tower state, including target acquisition and firing
void tower_draw(void);

#endif
