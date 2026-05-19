#include "game.h"
#include "video.h"
#include <lcom/lcf.h>
#include "../assets/hero.xpm"
#include <stdlib.h>
#include <string.h>

#define SCREEN_W 1152
#define SCREEN_H  864

static uint8_t     *sprite_map = NULL;
static xpm_image_t  sprite_img;

void game_reset(void) {
    if (sprite_map != NULL) {
        free(sprite_map);
        sprite_map = NULL;
    }
    sprite_map = xpm_load((xpm_map_t) hero, XPM_8_8_8_8, &sprite_img);
}

void game_tick(void) {}

void game_draw(void) {
    if (sprite_map == NULL) return;

    unsigned bpp = get_bytes_per_pixel();
    uint16_t x0 = (SCREEN_W - sprite_img.width)  / 2;
    uint16_t y0 = (SCREEN_H - sprite_img.height) / 2;
    uint8_t *ptr = sprite_map;

    for (int i = 0; i < sprite_img.height; i++) {
        for (int j = 0; j < sprite_img.width; j++) {
            uint32_t color = 0;
            memcpy(&color, ptr, bpp);
            ptr += bpp;
            vg_draw_pixel(x0 + j, y0 + i, color);
        }
    }
}
