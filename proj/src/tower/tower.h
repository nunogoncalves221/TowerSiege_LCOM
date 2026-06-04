#ifndef TOWER_H
#define TOWER_H

#include <lcom/lcf.h>
#include "sprite.h"
#include "map.h"
#include "enemy.h"

// types 

typedef enum { TOWER_NONE, TOWER_ARCHER, TOWER_MAGE } tower_type_t;
typedef enum { UI_NONE, UI_BUILD, UI_UPGRADE } ui_state_t;

// which button is highlighted in the tower popup
typedef enum {
    POPUP_NONE,      // popup is closed
    POPUP_UPGRADE,   // "Upgrade" button
    POPUP_REMOVE     // "Remove" button
} tower_popup_btn_t;

// constants

// tower costs
#define TOWER_ARCHER_COST 150
#define TOWER_MAGE_COST   300
#define TOWER_ARCHER_UPG_COST 100
#define TOWER_MAGE_UPG_COST   250

// archer tower stats (level 1)
#define TOWER_ARCHER_BASE_DMG       10
#define TOWER_ARCHER_BASE_RANGE     200
#define TOWER_ARCHER_BASE_COOLDOWN  55

// mage tower stats (level 1)
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

// data structures

typedef struct {
    tower_type_t type;
    int level;
    sprite_t *sp;
    int col, row;
    int range;
    uint32_t range_sq;
    int damage;
    int attack_cooldown;
    int attack_timer;
} tower_t;

typedef struct {
    int col, row;
    tower_t *tower;
} tower_slot_t;

// function prototypes

void tower_init(void);
void tower_handle_click_px(int px, int py);
void tower_tick(enemy_t **enemies, int enemy_count);
void tower_draw(void);

bool tower_popup_is_open(void);
bool tower_popup_handle_key(uint8_t scancode); // returns true if key was consumed
void tower_popup_draw(void);
void tower_popup_close(void);

#endif
