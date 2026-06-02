#ifndef __ANIMATED_SPRITE_H__
#define __ANIMATED_SPRITE_H__

#include <lcom/lcf.h>
#include <stdint.h>
#include "sprite.h"

typedef struct {
  sprite_t *sp;       // the base sprite we use to draw
  int aspeed;       // how many game ticks per animation frame
  int cur_aspeed;   // ticks left until we switch to the next frame
  int num_fig;      // total amount of frames in this animation
  int cur_fig;      // the frame we are currently showing
  uint32_t **map;   // array holding the pixel maps for each frame
} AnimSprite;

AnimSprite *create_animated_sprite(xpm_map_t *xpm_array, uint8_t no_pic, int aspeed);
void destroy_animated_sprite(AnimSprite *as);
int draw_animated_sprite(AnimSprite *as, uint16_t x, uint16_t y);

// stops at the last frame (great for attacks or dying)
void animate_sprite_once(AnimSprite *as);

// loops back to the first frame (great for walking or idle)
void animate_sprite_loop(AnimSprite *as);

#endif
