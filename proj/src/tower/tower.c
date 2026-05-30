#include "tower.h"
#include "sprite.h"
#include "../assets/archer_1.xpm"
#include "../assets/mage_1.xpm"

#define MAX_SLOTS  11
#define MAX_TOWERS 11

static tower_slot_t slots[MAX_SLOTS];
static int slot_count = 0;

static tower_t towers[MAX_TOWERS];
static int tower_count = 0;

static tower_type_t selected_tower_type = TOWER_NONE;

static sprite_t *archer_sprite = NULL;
static sprite_t *mage_sprite   = NULL;

void tower_init(void) {
    slot_count  = 0;
    tower_count = 0;
    selected_tower_type = TOWER_NONE;

    if (archer_sprite == NULL)
        archer_sprite = create_sprite((xpm_map_t) archer_1);
    if (mage_sprite == NULL)
        mage_sprite = create_sprite((xpm_map_t) mage_1);

    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            if (map_get_tile(c, r) != TILE_BLANK) continue;
            if (r == 8 && (c == 5 || c == 6)) continue;
            slots[slot_count++] = (tower_slot_t){c, r, NULL};
        }
    }
}

void tower_handle_click(int col, int row, int *gold) {
    if (row == 8 && col == 5) { selected_tower_type = TOWER_ARCHER; return; }
    if (row == 8 && col == 6) { selected_tower_type = TOWER_MAGE;   return; }

    if (selected_tower_type != TOWER_NONE && tower_count < MAX_TOWERS) {
        int cost = (selected_tower_type == TOWER_ARCHER) ? TOWER_ARCHER_COST : TOWER_MAGE_COST;
        if (*gold < cost) { selected_tower_type = TOWER_NONE; return; }

        for (int i = 0; i < slot_count; i++) {
            if (slots[i].col == col && slots[i].row == row && slots[i].tower == NULL) {
                *gold -= cost;
                tower_t *t = &towers[tower_count++];
                t->type = selected_tower_type;
                t->level = 1;
                t->col   = col;
                t->row   = row;
                t->sp    = (selected_tower_type == TOWER_ARCHER) ? archer_sprite : mage_sprite;
                if (selected_tower_type == TOWER_ARCHER) {
                    t->range = 200; t->damage = 10; t->attack_cooldown = 30;
                } else {
                    t->range = 150; t->damage = 25; t->attack_cooldown = 60;
                }
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

void tower_tick(void) {}

void tower_draw(void) {
    for (int i = 0; i < tower_count; i++) {
        draw_sprite(towers[i].sp, towers[i].col * TILE_SIZE, towers[i].row * TILE_SIZE);
    }
}
