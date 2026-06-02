#ifndef TOWER_H
#define TOWER_H

#include <lcom/lcf.h>
#include "../sprite/sprite.h"
#include "../map/map.h"
#include "../enemy/enemy.h"

typedef enum { TOWER_NONE, TOWER_ARCHER, TOWER_MAGE } tower_type_t;
typedef enum { UI_NONE, UI_BUILD, UI_UPGRADE } ui_state_t;

#define TOWER_ARCHER_COST 200
#define TOWER_MAGE_COST   350

// upgrade costs
#define TOWER_ARCHER_UPG_COST 150
#define TOWER_MAGE_UPG_COST   250

typedef struct {
    tower_type_t type;
    int level;    // level of upgrade (1 or 2)
    sprite_t *sp;
    int col, row; // position on the grid
    int range; // attack range
    int damage; // amount of damage per shot
    int attack_cooldown; // frames between shots
    int attack_timer; // timer to shoot
} tower_t;

typedef struct {
    int col, row; // position
    tower_t *tower; // NULL = empty slot
} tower_slot_t;

void tower_init(void);
void tower_handle_click(int col, int row);
// receives the enemies from the main game loop
void tower_tick(enemy_t **enemies, int enemy_count); 
void tower_draw(void);

#endif
