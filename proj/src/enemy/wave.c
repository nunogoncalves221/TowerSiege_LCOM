#include "wave.h"
#include "game.h"
#include "map.h"

#include "../assets/orc.xpm"

static int wave_number = 0;
static int enemies_to_spawn = 0;
static int spawn_timer = 0;
static int wave_delay = 0;

static int16_t enemy_start_x = 0;
static int16_t enemy_start_y = 0;

static void start_wave(int wave) {
    wave_number = wave;
    enemies_to_spawn = 5 + (wave - 1) * 3; // wave 1->5, 2->8, 3->11, ...
    spawn_timer = 60; // 2-second lead-in before first enemy of each wave
    wave_delay = 0;
}

void wave_init(void) {
    enemy_t *probe = enemy_create((xpm_map_t) orc, 0, 0, 100, 2);
    if (probe != NULL) {
        enemy_start_x = -(int16_t)probe->sp->width;
        enemy_start_y = (int16_t)(TILE_SIZE * 1 + (TILE_SIZE / 2) - (int16_t)(probe->sp->height / 2));
        enemy_destroy(probe);
    }
    start_wave(1);
}

void wave_tick(enemy_t **enemies, int *enemy_count) {
    if (enemies_to_spawn > 0) {
        spawn_timer--;
        if (spawn_timer <= 0 && *enemy_count < MAX_ENEMIES) {
            enemy_t *e = enemy_create((xpm_map_t) orc, enemy_start_x, enemy_start_y, 100, 2);
            if (e != NULL) {
                enemies[(*enemy_count)++] = e;
                enemies_to_spawn--;
                int interval = 90 - (wave_number - 1) * 5;
                spawn_timer = interval < 45 ? 45 : interval;
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
