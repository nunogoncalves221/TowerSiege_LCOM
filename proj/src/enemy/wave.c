#include "wave.h"
#include "game.h"
#include "map.h"
#include <stdlib.h>

#include "../assets/orc.xpm"
#include "../assets/goblin.xpm"

static int wave_number = 0;
static int enemies_to_spawn = 0;
static int spawn_timer = 0;
static int wave_delay = 0;

static int16_t orc_start_x = 0;
static int16_t orc_start_y = 0;
static int16_t goblin_start_x = 0;
static int16_t goblin_start_y = 0;

static void start_wave(int wave) {
    wave_number = wave;
    enemies_to_spawn = 5 + (wave - 1) * 3; // number of enemies per wave
    spawn_timer = 30; // 0.5-second lead-in before first enemy of each wave
    wave_delay = 0;
}

void wave_init(void) {
    enemy_t *probe = enemy_create((xpm_map_t) orc, 0, 0, 100, 2);
    if (probe != NULL) {
        orc_start_x = -(int16_t)probe->sp->width;
        orc_start_y = (int16_t)(TILE_SIZE * 1 + (TILE_SIZE / 2) - (int16_t)(probe->sp->height / 2));
        enemy_destroy(probe);
    }
    probe = enemy_create((xpm_map_t) goblin, 0, 0, 50, 4);
    if (probe != NULL) {
        goblin_start_x = -(int16_t)probe->sp->width;
        goblin_start_y = (int16_t)(TILE_SIZE * 1 + (TILE_SIZE / 2) - (int16_t)(probe->sp->height / 2));
        enemy_destroy(probe);
    }
    start_wave(1);
}

void wave_tick(enemy_t **enemies, int *enemy_count) {
    if (enemies_to_spawn > 0) {
        spawn_timer--;
        if (spawn_timer <= 0 && *enemy_count < MAX_ENEMIES) {
            // orc spawn chance grows 20% per wave, capped at 80%
            int orc_chance = (wave_number - 1) * 20;
            if (orc_chance > 80) orc_chance = 80;

            enemy_t *e;
            if (rand() % 100 < orc_chance) {
                e = enemy_create((xpm_map_t) orc, orc_start_x, orc_start_y, 100, 2);
            } else {
                e = enemy_create((xpm_map_t) goblin, goblin_start_x, goblin_start_y, 50, 4);
            }
            if (e != NULL) {
                enemies[(*enemy_count)++] = e;
                enemies_to_spawn--;
                int interval = 50 - (wave_number - 1) * 3;
                spawn_timer = interval < 20 ? 20 : interval;
            }
        }
    } else if (*enemy_count == 0) {
        wave_delay++;
        if (wave_delay >= WAVE_DELAY) {
            start_wave(wave_number + 1);
        }
    }
}

int wave_get_number(void) {
    return wave_number;
}
