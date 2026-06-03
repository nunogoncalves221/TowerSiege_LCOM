#include "tower.h"
#include "bullet.h"
#include "sprite.h"
#include "game.h"
#include "../enemy/enemy.h"
#include "../assets/archer_1.xpm"
#include "../assets/mage_1.xpm"
#include "../assets/archer_2.xpm"
#include "../assets/mage_2.xpm"
#include "../assets/bullet.xpm"
#include "../assets/orb1.xpm"
#include "../assets/orb2.xpm"

#define MAX_SLOTS  11
#define MAX_TOWERS 11

static tower_slot_t slots[MAX_SLOTS];
static int slot_count = 0;

static tower_t towers[MAX_TOWERS];
static int tower_count = 0;

static tower_type_t selected_tower_type = TOWER_NONE;

// sprites for both levels
static sprite_t *archer_sprite = NULL;
static sprite_t *mage_sprite   = NULL;
static sprite_t *archer_2_sprite = NULL;
static sprite_t *mage_2_sprite   = NULL;
static sprite_t *bullet_sprite   = NULL;
static sprite_t *orb1_sprite     = NULL;
static sprite_t *orb2_sprite     = NULL;

void tower_init(void) {
    slot_count  = 0;
    tower_count = 0;
    selected_tower_type = TOWER_NONE;

    // load all sprites into memory safely
    if (archer_sprite == NULL)   archer_sprite   = create_sprite((xpm_map_t) archer_1);
    if (mage_sprite == NULL)     mage_sprite     = create_sprite((xpm_map_t) mage_1);
    if (archer_2_sprite == NULL) archer_2_sprite = create_sprite((xpm_map_t) archer_2);
    if (mage_2_sprite == NULL)   mage_2_sprite   = create_sprite((xpm_map_t) mage_2);
    if (bullet_sprite == NULL) bullet_sprite = create_sprite((xpm_map_t) bullet);
    if (orb1_sprite == NULL)   orb1_sprite   = create_sprite((xpm_map_t) orb1);
    if (orb2_sprite == NULL)   orb2_sprite   = create_sprite((xpm_map_t) orb2);
    bullet_init();

    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            if (map_get_tile(c, r) != TILE_BLANK) continue;
            if (r == 8 && (c == 5 || c == 6)) continue; // ignore ui area
            slots[slot_count++] = (tower_slot_t){c, r, NULL};
        }
    }
}

void tower_handle_click(int col, int row) {
    // check ui selection (original team code)
    if (row == 8 && col == 5) { selected_tower_type = TOWER_ARCHER; return; }
    if (row == 8 && col == 6) { selected_tower_type = TOWER_MAGE;   return; }

    // upgrade logic: check if we clicked an existing tower to upgrade it
    for (int i = 0; i < slot_count; i++) {
        if (slots[i].col == col && slots[i].row == row && slots[i].tower != NULL) {
            tower_t *t = slots[i].tower;
            
            // only upgrade if it's currently level 1
            if (t->level == 1) {
                int upg_cost = (t->type == TOWER_ARCHER) ? TOWER_ARCHER_UPG_COST : TOWER_MAGE_UPG_COST;
                
                // check if we can actually afford the upgrade
                if (get_player_gold() >= upg_cost) {
                    add_player_gold(-upg_cost); // pay up
                    t->level = 2;
                    
                    // apply stats and new sprite
                    if (t->type == TOWER_ARCHER) {
                        t->sp = archer_2_sprite;
                        t->damage += TOWER_ARCHER_UPG_DMG_INC;
                        t->range += TOWER_ARCHER_UPG_RANGE_INC;
                        t->attack_cooldown -= TOWER_ARCHER_UPG_CD_DEC;

                        t->range_sq = t->range * t->range; // update the cache
                    } else {
                        t->sp = mage_2_sprite;
                        t->damage += TOWER_MAGE_UPG_DMG_INC;
                        t->range += TOWER_MAGE_UPG_RANGE_INC;
                        
                        t->range_sq = t->range * t->range; // update the cache
                    }
                    
                    // tell the game to redraw the map cache so the new sprite appears
                    map_mark_dirty(); 
                }
            }
            selected_tower_type = TOWER_NONE;
            return;
        }
    }

    // build logic 
    if (selected_tower_type != TOWER_NONE && tower_count < MAX_TOWERS) {
        int cost = (selected_tower_type == TOWER_ARCHER) ? TOWER_ARCHER_COST : TOWER_MAGE_COST;
        
        // cancel if we dont have gold for this
        if (get_player_gold() < cost) { 
            selected_tower_type = TOWER_NONE; 
            return; 
        }

        for (int i = 0; i < slot_count; i++) {
            if (slots[i].col == col && slots[i].row == row && slots[i].tower == NULL) {
                add_player_gold(-cost); // deduct the build cost
                
                tower_t *t = &towers[tower_count++];
                t->type = selected_tower_type;
                t->level = 1;
                t->col   = col;
                t->row   = row;
                t->sp    = (selected_tower_type == TOWER_ARCHER) ? archer_sprite : mage_sprite;
                
                // apply base stats
                if (selected_tower_type == TOWER_ARCHER) {
                    t->range = TOWER_ARCHER_BASE_RANGE; 
                    t->damage = TOWER_ARCHER_BASE_DMG; 
                    t->attack_cooldown = TOWER_ARCHER_BASE_COOLDOWN;
                } else {
                    t->range = TOWER_MAGE_BASE_RANGE; 
                    t->damage = TOWER_MAGE_BASE_DMG; 
                    t->attack_cooldown = TOWER_MAGE_BASE_COOLDOWN;
                }
                
                // pre-calculate the squared range once right after building
                t->range_sq = t->range * t->range;

                t->attack_timer = 0;
                slots[i].tower = t;
                selected_tower_type = TOWER_NONE;
                map_mark_dirty();
                return;
            }
        }
    }

    selected_tower_type = TOWER_NONE;
}

void tower_tick(enemy_t **enemies, int enemy_count) {
    // scan all active towers and make them shoot
    for (int i = 0; i < tower_count; i++) {
        tower_t *t = &towers[i];

        // if the tower is still reloading, just count down and move to the next one
        if (t->attack_timer > 0) {
            t->attack_timer--;
            continue; 
        }

        // calculate the exact center pixel of the tower for accurate measuring
        int tower_x = t->col * TILE_SIZE + (TILE_SIZE / 2);
        int tower_y = t->row * TILE_SIZE + (TILE_SIZE / 2);

        enemy_t *target = NULL;
        
        // use long to prevent overflow with large ranges on 16-bit int systems
        // just read the cached value
        long min_dist_sq = (long)t->range_sq;

        // find the closest active enemy within range
        for (int j = 0; j < enemy_count; j++) {
            enemy_t *e = enemies[j];
            if (e == NULL || !e->active) continue;

            int enemy_x = e->x + (e->sp->width / 2);
            int enemy_y = e->y + (e->sp->height / 2);

            int dx = enemy_x - tower_x;
            int dy = enemy_y - tower_y;
            long dist_sq = (long)dx * dx + (long)dy * dy;

            // lock onto the first enemy found that is closest
            if (dist_sq < min_dist_sq) {
                min_dist_sq = dist_sq;
                target = e;
            }
        }

        // if we find a target, fire
        if (target != NULL) {
            if (t->type == TOWER_ARCHER) {
                bullet_fire((int16_t)tower_x, (int16_t)tower_y,
                            target, t->damage, bullet_sprite);
            } else {
                sprite_t *orb;
                if (t->level == 2) {
                    orb = orb2_sprite;
                } else {
                    orb = orb1_sprite;
                }
                bullet_fire((int16_t)tower_x, (int16_t)tower_y,
                            target, t->damage, orb);
            }

            t->attack_timer = t->attack_cooldown;
        }
    }
}

void tower_draw(void) {
    for (int i = 0; i < tower_count; i++) {
        draw_sprite(towers[i].sp, towers[i].col * TILE_SIZE, towers[i].row * TILE_SIZE);
    }
}
