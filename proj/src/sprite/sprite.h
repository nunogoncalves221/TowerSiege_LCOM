#ifndef SPRITE_H
#define SPRITE_H

#include <lcom/lcf.h>

// as recommended in the lectures, a sprite only holds the visual representation
typedef struct {
    uint16_t width;
    uint16_t height;
    uint32_t *map; // holds the actual pixel colors
} sprite_t;

// allocates memory and loads the XPM into the sprite
sprite_t* create_sprite(xpm_map_t xpm);

// frees the allocated memory to prevent memory leaks
void destroy_sprite(sprite_t *sp);

// draws the sprite at the given coordinates, ignoring the transparent color
int draw_sprite(sprite_t *sp, int16_t x, int16_t y);

unsigned (vg_get_x_res)();
unsigned (vg_get_y_res)();

#endif
