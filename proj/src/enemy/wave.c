#include "wave.h"
#include "game.h"
#include "map.h"
#include <stdlib.h>

#include "../assets/orc.xpm"
#include "../assets/goblin.xpm"

static int wave_number = 0;
static int goblins_to_spawn = 0;
static int orcs_to_spawn = 0;
static int spawn_timer = 0;
static int wave_delay = 0;

static int16_t orc_start_x = 0;
static int16_t orc_start_y = 0;
static int16_t goblin_start_x = 0;
static int16_t goblin_start_y = 0;

static void start_wave(int wave) {
    wave_number = wave;
    // each wave has 1.2x more enemies than the previous
    float g = 3.6f, o = 2.4f;
    for (int i = 1; i < wave; i++) {
        g *= 1.2f;
        o *= 1.2f;
    }
    goblins_to_spawn = (int)g; // wave 1:5  2:6  3:7  4:8  5:10  10:25 ...
    orcs_to_spawn    = (int)o; // wave 1:3  2:3  3:4  4:5  5:6   10:15 ...
    spawn_timer = 30;
    wave_delay = 0;
}

void wave_init(void) {
    enemy_t *probe = enemy_create((xpm_map_t) orc, 0, 0, 150, 2);
    if (probe != NULL) {
        orc_start_x = -(int16_t)probe->sp->width;
        orc_start_y = (int16_t)(TILE_SIZE * 1 + (TILE_SIZE / 2) - (int16_t)(probe->sp->height / 2));
        enemy_destroy(probe);
    }
    probe = enemy_create((xpm_map_t) goblin, 0, 0, 60, 3);
    if (probe != NULL) {
        goblin_start_x = -(int16_t)probe->sp->width;
        goblin_start_y = (int16_t)(TILE_SIZE * 1 + (TILE_SIZE / 2) - (int16_t)(probe->sp->height / 2));
        enemy_destroy(probe);
    }
    start_wave(1);
}

void wave_tick(enemy_t **enemies, int *enemy_count) {
    if (goblins_to_spawn > 0 || orcs_to_spawn > 0) {
        spawn_timer--;
        if (spawn_timer <= 0 && *enemy_count < MAX_ENEMIES) {
            enemy_t *e;
            // pick randomly from whichever types still remain
            int total = goblins_to_spawn + orcs_to_spawn;
            if (orcs_to_spawn > 0 && (goblins_to_spawn == 0 || rand() % total < orcs_to_spawn)) {
                e = enemy_create((xpm_map_t) orc, orc_start_x, orc_start_y, 150, 2);
                if (e != NULL) { e->gold_value = 30; orcs_to_spawn--; }
            } else {
                e = enemy_create((xpm_map_t) goblin, goblin_start_x, goblin_start_y, 60, 3);
                if (e != NULL) { e->gold_value = 20; goblins_to_spawn--; }
            }
            if (e != NULL) {
                enemies[(*enemy_count)++] = e;
                int interval = 50 - (wave_number - 1) * 3;
                spawn_timer = interval < 20 ? 20 : interval;
            }
        }
    } else {
        // countdown starts when the last enemy spawns
        wave_delay++;
        if (*enemy_count == 0 || wave_delay >= WAVE_DELAY) {
            start_wave(wave_number + 1);
        }
    }
}

int wave_get_number(void) {
    return wave_number;
}
