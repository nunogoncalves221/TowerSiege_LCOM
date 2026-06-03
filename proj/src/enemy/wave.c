#include "wave.h"
#include "game.h"
#include "map.h"
#include <stdlib.h>
#include <stdio.h>

#include "../assets/orc.xpm"
#include "../assets/goblin.xpm"

static int wave_number = 0;
static int goblins_to_spawn = 0;
static int orcs_to_spawn = 0;
static int spawn_timer = 0;
static int spawn_interval = 0;
static int wave_delay = 0;

static int16_t orc_start_x = 0;
static int16_t orc_start_y = 0;
static int16_t goblin_start_x = 0;
static int16_t goblin_start_y = 0;

static void start_wave(int wave) {
    wave_number = wave;
    float g = 3.6, o = 2.4;
    // enemy count multiplier
    for (int i = 1; i < wave; i++) {
        g *= 1.2;
        o *= 1.2;
    }
    goblins_to_spawn = (int)g;
    orcs_to_spawn    = (int)o;
    int interval = 50 - (wave - 1) * 3;
    spawn_interval = interval < 20 ? 20 : interval;
    spawn_timer = 30;
    wave_delay = 0;
}

void wave_init(void) {
    int w, h;

    // get sprite dimensions and set spawn coordenates
    sscanf(orc[0], "%d %d", &w, &h);
    orc_start_x = -(int16_t)w;
    orc_start_y = (int16_t)(TILE_SIZE + TILE_SIZE / 2 - h / 2);

    sscanf(goblin[0], "%d %d", &w, &h);
    goblin_start_x = -(int16_t)w;
    goblin_start_y = (int16_t)(TILE_SIZE + TILE_SIZE / 2 - h / 2);

    start_wave(1);
}

void wave_tick(enemy_t **enemies, int *enemy_count) {
    if (goblins_to_spawn > 0 || orcs_to_spawn > 0) {
        spawn_timer--;
        if (spawn_timer <= 0 && *enemy_count < MAX_ENEMIES) {
            enemy_t *e;
            int total = goblins_to_spawn + orcs_to_spawn;

            // choose randomly between goblin and orc
            if (orcs_to_spawn > 0 && (goblins_to_spawn == 0 || rand() % total < orcs_to_spawn)) {
                e = enemy_create((xpm_map_t) orc, orc_start_x, orc_start_y, 150, 2);
                if (e != NULL) { e->gold_value = 30; orcs_to_spawn--; }
            } else {
                e = enemy_create((xpm_map_t) goblin, goblin_start_x, goblin_start_y, 60, 3);
                if (e != NULL) { e->gold_value = 25; goblins_to_spawn--; }
            }
            if (e != NULL) {
                enemies[(*enemy_count)++] = e; // add enemy to array
                spawn_timer = spawn_interval; // reset spawn timer
            }
        }
    } else {
        // start countdown after last enemy spawns
        wave_delay++;
        if (*enemy_count == 0 || wave_delay >= WAVE_DELAY) {
            start_wave(wave_number + 1);
        }
    }
}

int wave_get_number(void) {
    return wave_number;
}
