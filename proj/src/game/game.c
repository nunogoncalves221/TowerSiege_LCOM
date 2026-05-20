#include "game.h"
#include "sprite.h"
#include "video.h"
#include "../assets/hero.xpm"

static sprite_t *hero_sprite = NULL;

void game_reset(void) {
    destroy_sprite(hero_sprite);
    hero_sprite = create_sprite((xpm_map_t) hero);
}

void game_tick(void) {}

void game_draw(void) {
    if (hero_sprite == NULL) return;

    int16_t x = (int16_t)((vg_get_x_res() - hero_sprite->width)  / 2);
    int16_t y = (int16_t)((vg_get_y_res() - hero_sprite->height) / 2);
    draw_sprite(hero_sprite, x, y);
}
