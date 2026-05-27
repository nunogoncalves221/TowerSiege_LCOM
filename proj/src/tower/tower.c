#include "tower.h"
#define MAX_SLOTS 11

static tower_slot_t slots[MAX_SLOTS]; // array of tower slots
static int slot_count = 0; // number of slots found on the map
static ui_state_t ui_state = UI_NONE; // tracks panel state
static int selected_slot = -1; // index of selected slot

void tower_init(void) {
    slot_count = 0;
    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            if (map_get_tile(c, r) == TILE_BLANK) {
                slots[slot_count++] = (tower_slot_t){c, r, NULL};
            }
        }
    }
}

void tower_handle_click(int col, int row) {
    for (int i = 0; i < slot_count; i++) {
        if (slots[i].col == col && slots[i].row == row) {
            selected_slot = i;
            ui_state = (slots[i].tower == NULL) ? UI_BUILD : UI_UPGRADE;
            return;
        }
    }
    ui_state = UI_NONE;
    selected_slot = -1;
}

void tower_tick(void) {}

void tower_draw(void) {}
