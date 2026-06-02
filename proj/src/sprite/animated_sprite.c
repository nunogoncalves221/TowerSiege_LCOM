#include "animated_sprite.h"

AnimSprite *create_animated_sprite(xpm_map_t *xpm_array, uint8_t no_pic, int aspeed) {
  AnimSprite *asp = (AnimSprite *)malloc(sizeof(AnimSprite));
  
  if (asp == NULL) return NULL;
  if (xpm_array == NULL || no_pic == 0) {
    free(asp);
    return NULL;
  }

  // grab the first frame to setup width, height, etc
  asp->sp = create_sprite(xpm_array[0]);
  if (asp->sp == NULL) {
    free(asp);
    return NULL;
  }

  asp->map = (uint32_t **)malloc(no_pic * sizeof(uint32_t *));
  if (asp->map == NULL) {
    destroy_sprite(asp->sp);
    free(asp);
    return NULL;
  }

  asp->map[0] = asp->sp->map;

  // load the rest of the frames into the map array
  for (int i = 1; i < no_pic; i++) {
    if (xpm_array[i] == NULL) {
      for (int j = 1; j < i; j++) free(asp->map[j]);
      free(asp->map);
      destroy_sprite(asp->sp);
      free(asp);
      return NULL;
    }
    
    xpm_image_t img;
    asp->map[i] = (uint32_t *) xpm_load(xpm_array[i], XPM_8_8_8_8, &img);
    
    // safety check to ensure all frames have the exact same size
    if (asp->map[i] == NULL || img.width != asp->sp->width || img.height != asp->sp->height) {
      for (int j = 1; j < i; j++) free(asp->map[j]);
      free(asp->map);
      destroy_sprite(asp->sp);
      free(asp);
      return NULL;
    }
  }

  asp->aspeed = aspeed;
  asp->cur_aspeed = aspeed;
  asp->num_fig = no_pic;
  asp->cur_fig = 0;

  return asp;
}

void destroy_animated_sprite(AnimSprite *as) {
  if (as == NULL) return;

  if (as->map != NULL) {
    for (int i = 1; i < as->num_fig; i++) {
      if (as->map[i] != NULL) free(as->map[i]);
    }
    free(as->map);
  }

  if (as->sp != NULL) destroy_sprite(as->sp);
  free(as);
}

int draw_animated_sprite(AnimSprite *as, uint16_t x, uint16_t y) {
  if (as == NULL || as->sp == NULL || as->map == NULL) return 1;

  // hot-swap the pixel map to the current animation frame, draw it, and swap it back
  uint32_t *original_map = as->sp->map;
  as->sp->map = as->map[as->cur_fig];
  
  int result = draw_sprite(as->sp, x, y);
  
  as->sp->map = original_map;
  return result;
}

void animate_sprite_once(AnimSprite *as) {
  if (as == NULL) return;

  as->cur_aspeed--;
  
  if (as->cur_aspeed <= 0) {
    as->cur_fig++;
    // clamp it at the last frame so it doesn't loop
    if (as->cur_fig >= as->num_fig) {
      as->cur_fig = as->num_fig - 1; 
    } else {
      as->cur_aspeed = as->aspeed;
    }
  }
}

void animate_sprite_loop(AnimSprite *as) {
  if (as == NULL) return;

  as->cur_aspeed--;
  
  if (as->cur_aspeed <= 0) {
    as->cur_fig++;
    // loop back around to frame 0
    if (as->cur_fig >= as->num_fig) {
      as->cur_fig = 0; 
    }
    as->cur_aspeed = as->aspeed;
  }
}
