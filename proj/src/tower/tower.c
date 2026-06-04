#include "tower.h"
#include "bullet.h"
#include "sprite.h"
#include "game.h"
#include "player.h"
#include "font.h"
#include "video.h"
#include "menu_nav.h"

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

static sprite_t *archer_sprite = NULL, *mage_sprite     = NULL;
static sprite_t *archer_2_sprite = NULL, *mage_2_sprite = NULL;
static sprite_t *bullet_sprite = NULL, *orb1_sprite = NULL, *orb2_sprite = NULL;

// popup state: -1 = closed, >=0 = slot index with popup open
static int popup_slot = -1;
static int popup_selected = 0; // 0 = upgrade, 1 = remove
static int popup_px = 0, popup_py = 0;

#define COL_POPUP_BG       0x00222222
#define COL_POPUP_BORDER   0x00AAAAAA
#define COL_BTN_UPG        0x00116611
#define COL_BTN_UPG_HOVER  0x0022AA22
#define COL_BTN_DISABLED   0x00333333
#define COL_BTN_REM        0x00771111
#define COL_BTN_REM_HOVER  0x00AA2222
#define COL_TEXT_GOLD      0x00FFD700
#define COL_TEXT_GREY      0x00888888
#define COL_TEXT_WHITE     0x00FFFFFF

static void fill_rect(int x, int y, int w, int h, uint32_t c) {
    int sw = (int)vg_get_x_res(), sh = (int)vg_get_y_res();
    for (int py = y; py < y+h; py++)
        for (int px = x; px < x+w; px++)
            if (px>=0 && px<sw && py>=0 && py<sh) vg_draw_pixel(px, py, c);
}

static void border_rect(int x, int y, int w, int h, uint32_t c) {
    int sw = (int)vg_get_x_res(), sh = (int)vg_get_y_res();
    for (int px = x; px < x+w; px++) {
        if (px<0||px>=sw) continue;
        if (y>=0&&y<sh)         vg_draw_pixel(px, y,     c);
        if (y+h-1>=0&&y+h-1<sh) vg_draw_pixel(px, y+h-1, c);
    }
    for (int py = y; py < y+h; py++) {
        if (py<0||py>=sh) continue;
        if (x>=0&&x<sw)         vg_draw_pixel(x,     py, c);
        if (x+w-1>=0&&x+w-1<sw) vg_draw_pixel(x+w-1, py, c);
    }
}

void tower_init(void) {
    slot_count = tower_count = 0;
    selected_tower_type = TOWER_NONE;
    popup_slot = -1;

    if (!archer_sprite)   archer_sprite   = create_sprite((xpm_map_t)archer_1);
    if (!mage_sprite)     mage_sprite     = create_sprite((xpm_map_t)mage_1);
    if (!archer_2_sprite) archer_2_sprite = create_sprite((xpm_map_t)archer_2);
    if (!mage_2_sprite)   mage_2_sprite   = create_sprite((xpm_map_t)mage_2);
    if (!bullet_sprite)   bullet_sprite   = create_sprite((xpm_map_t)bullet);
    if (!orb1_sprite)     orb1_sprite     = create_sprite((xpm_map_t)orb1);
    if (!orb2_sprite)     orb2_sprite     = create_sprite((xpm_map_t)orb2);
    bullet_init();

    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++) {
            if (map_get_tile(c, r) != TILE_BLANK) continue;
            if (r == 8 && (c == 5 || c == 6)) continue;
            slots[slot_count++] = (tower_slot_t){c, r, NULL};
        }
}

bool tower_popup_is_open(void) { return popup_slot >= 0; }
void tower_popup_close(void)   { popup_slot = -1; }

// returns true if the popup consumed the key (so game.c skips hero movement)
bool tower_popup_handle_key(uint8_t scancode) {
    if (popup_slot < 0) return false;

    if (scancode == 0x01) { // Escape
        popup_slot = -1;
        return true;
    }

    if (menu_nav_handle_key(scancode, &popup_selected, 2)) {
        // Enter pressed — execute selected action
        tower_t *t = slots[popup_slot].tower;

        if (popup_selected == 0 && t && t->level == 1) { // upgrade
            int cost = (t->type == TOWER_ARCHER) ? TOWER_ARCHER_UPG_COST : TOWER_MAGE_UPG_COST;
            if (player_spend_gold(cost)) {
                t->level = 2;
                if (t->type == TOWER_ARCHER) {
                    t->sp = archer_2_sprite;
                    t->damage += TOWER_ARCHER_UPG_DMG_INC;
                    t->range  += TOWER_ARCHER_UPG_RANGE_INC;
                    t->attack_cooldown -= TOWER_ARCHER_UPG_CD_DEC;
                } else {
                    t->sp = mage_2_sprite;
                    t->damage += TOWER_MAGE_UPG_DMG_INC;
                    t->range  += TOWER_MAGE_UPG_RANGE_INC;
                }
                t->range_sq = t->range * t->range;
                map_mark_dirty();
            }
        } else if (popup_selected == 1 && t) { // remove
            int refund = (t->type == TOWER_ARCHER) ? (int)(TOWER_ARCHER_COST * 0.3f)
                                                   : (int)(TOWER_MAGE_COST   * 0.3f);
            player_add_gold(refund);
            for (int i = 0; i < tower_count; i++) {
                if (&towers[i] == t) {
                    towers[i] = towers[--tower_count];
                    for (int s = 0; s < slot_count; s++)
                        if (slots[s].tower == &towers[tower_count])
                            slots[s].tower = &towers[i];
                    break;
                }
            }
            slots[popup_slot].tower = NULL;
            map_mark_dirty();
        }
        popup_slot = -1;
    }
    return true;
}

void tower_handle_click_px(int px, int py) {
    if (popup_slot >= 0) { popup_slot = -1; return; }

    int col = px / TILE_SIZE, row = py / TILE_SIZE;

    if (row == 8 && col == 5) { selected_tower_type = TOWER_ARCHER; return; }
    if (row == 8 && col == 6) { selected_tower_type = TOWER_MAGE;   return; }

    for (int i = 0; i < slot_count; i++) {
        if (slots[i].col == col && slots[i].row == row && slots[i].tower) {
            popup_slot = i;
            popup_selected = 0;
            popup_px = col * TILE_SIZE;
            popup_py = row * TILE_SIZE - 84;
            int sw = (int)vg_get_x_res(), sh = (int)vg_get_y_res();
            if (popup_px + 260 > sw) popup_px = sw - 262;
            if (popup_px < 0) popup_px = 2;
            if (popup_py < 0) popup_py = row * TILE_SIZE + TILE_SIZE + 4;
            if (popup_py + 80 > sh) popup_py = sh - 82;
            selected_tower_type = TOWER_NONE;
            return;
        }
    }

    if (selected_tower_type == TOWER_NONE || tower_count >= MAX_TOWERS) {
        selected_tower_type = TOWER_NONE; return;
    }
    int cost = (selected_tower_type == TOWER_ARCHER) ? TOWER_ARCHER_COST : TOWER_MAGE_COST;
    if (player_get_gold() < cost) { selected_tower_type = TOWER_NONE; return; }

    for (int i = 0; i < slot_count; i++) {
        if (slots[i].col == col && slots[i].row == row && !slots[i].tower) {
            player_add_gold(-cost);
            tower_t *t = &towers[tower_count++];
            t->type = selected_tower_type; t->level = 1;
            t->col = col; t->row = row;
            t->sp = (selected_tower_type == TOWER_ARCHER) ? archer_sprite : mage_sprite;
            if (selected_tower_type == TOWER_ARCHER) {
                t->range = TOWER_ARCHER_BASE_RANGE;
                t->damage = TOWER_ARCHER_BASE_DMG;
                t->attack_cooldown = TOWER_ARCHER_BASE_COOLDOWN;
            } else {
                t->range = TOWER_MAGE_BASE_RANGE;
                t->damage = TOWER_MAGE_BASE_DMG;
                t->attack_cooldown = TOWER_MAGE_BASE_COOLDOWN;
            }
            t->range_sq = t->range * t->range;
            t->attack_timer = 0;
            slots[i].tower = t;
            selected_tower_type = TOWER_NONE;
            map_mark_dirty();
            return;
        }
    }
    selected_tower_type = TOWER_NONE;
}

void tower_tick(enemy_t **enemies, int enemy_count) {
    for (int i = 0; i < tower_count; i++) {
        tower_t *t = &towers[i];
        if (t->attack_timer > 0) { t->attack_timer--; continue; }

        int tx = t->col * TILE_SIZE + TILE_SIZE/2;
        int ty = t->row * TILE_SIZE + TILE_SIZE/2;
        enemy_t *target = NULL;
        long min_sq = (long)t->range_sq;

        for (int j = 0; j < enemy_count; j++) {
            enemy_t *e = enemies[j];
            if (!e || !e->active) continue;
            int dx = (e->x + e->sp->width/2)  - tx;
            int dy = (e->y + e->sp->height/2) - ty;
            long d = (long)dx*dx + (long)dy*dy;
            if (d < min_sq) { min_sq = d; target = e; }
        }

        if (target) {
            sprite_t *proj = (t->type == TOWER_ARCHER) ? bullet_sprite
                           : (t->level == 2 ? orb2_sprite : orb1_sprite);
            bullet_fire((int16_t)tx, (int16_t)ty, target, t->damage, proj);
            t->attack_timer = t->attack_cooldown;
        }
    }
}

void tower_draw(void) {
    for (int i = 0; i < tower_count; i++)
        draw_sprite(towers[i].sp, towers[i].col * TILE_SIZE, towers[i].row * TILE_SIZE);
}

void tower_popup_draw(void) {
    if (popup_slot < 0) return;
    tower_t *t = slots[popup_slot].tower;
    if (!t) { popup_slot = -1; return; }

    fill_rect(popup_px, popup_py, 260, 80, COL_POPUP_BG);
    border_rect(popup_px, popup_py, 260, 80, COL_POPUP_BORDER);

    int bx = popup_px + 8, bw = 244;
    int uy = popup_py + 8, ry = uy + 36;

    // upgrade button
    bool can_upg    = (t->level == 1);
    int  upg_cost   = (t->type == TOWER_ARCHER) ? TOWER_ARCHER_UPG_COST : TOWER_MAGE_UPG_COST;
    bool can_afford = (player_get_gold() >= upg_cost);
    fill_rect(bx, uy, bw, 28, (!can_upg || !can_afford) ? COL_BTN_DISABLED
                              : (popup_selected == 0    ? COL_BTN_UPG_HOVER : COL_BTN_UPG));
    border_rect(bx, uy, bw, 28, COL_POPUP_BORDER);
    if (!can_upg) {
        uint16_t lw = font_string_width("max level");
        font_draw_string((uint16_t)(bx+(bw-lw)/2), (uint16_t)(uy+7), "max level", COL_TEXT_GREY);
    } else {
        char label[24];
        snprintf(label, sizeof(label), "upgrade -%d g", upg_cost);
        uint16_t lw = font_string_width(label);
        font_draw_string((uint16_t)(bx+(bw-lw)/2), (uint16_t)(uy+7),
                         label, can_afford ? COL_TEXT_GOLD : COL_TEXT_GREY);
    }
    if (popup_selected == 0)
        menu_nav_draw_item((uint16_t)(bx+4), (uint16_t)(uy+7), "", true);

    // remove button
    int refund = (t->type == TOWER_ARCHER) ? (int)(TOWER_ARCHER_COST * 0.3f)
                                           : (int)(TOWER_MAGE_COST   * 0.3f);
    fill_rect(bx, ry, bw, 28, popup_selected == 1 ? COL_BTN_REM_HOVER : COL_BTN_REM);
    border_rect(bx, ry, bw, 28, COL_POPUP_BORDER);
    char rem_label[32];
    snprintf(rem_label, sizeof(rem_label), "remove +%d g", refund);
    uint16_t rw = font_string_width(rem_label);
    font_draw_string((uint16_t)(bx+(bw-rw)/2), (uint16_t)(ry+7), rem_label, COL_TEXT_WHITE);
    if (popup_selected == 1)
        menu_nav_draw_item((uint16_t)(bx+4), (uint16_t)(ry+7), "", true);
}
