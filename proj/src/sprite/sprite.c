#include "sprite.h"
#include "video.h" // to call vg_draw_pixel

sprite_t* create_sprite(xpm_map_t xpm) {
    // allocate memory for the struct itself
    sprite_t *sp = (sprite_t *) malloc(sizeof(sprite_t));
    if (sp == NULL) return NULL;

    // load the image. xpm_load allocates memory for the pixel map under the hood
    xpm_image_t img;
    
    // assuming direct color mode 0x14C (XPM_8_8_8_8)
    sp->map = (uint32_t *) xpm_load(xpm, XPM_8_8_8_8, &img);
    
    if (sp->map == NULL) {
        free(sp); // clean up if loading fails
        return NULL;
    }

    sp->width = img.width;
    sp->height = img.height;

    return sp;
}

void destroy_sprite(sprite_t *sp) {
    if (sp == NULL) return;
    
    // free the pixel map allocated by xpm_load
    if (sp->map != NULL) {
        free(sp->map);
    }
    
    // free the struct wrapper
    free(sp);
}

// update the signature in sprite.h as well!
int draw_sprite(sprite_t *sp, int16_t x, int16_t y) {
    if (sp == NULL || sp->map == NULL) return 1;

    uint32_t transparent = xpm_transparency_color(XPM_8_8_8_8);
    
    // get the screen resolutions from your video module
    int16_t screen_w = (int16_t) vg_get_x_res();
    int16_t screen_h = (int16_t) vg_get_y_res();

    for (int16_t i = 0; i < (int16_t) sp->height; i++) {
        for (int16_t j = 0; j < (int16_t) sp->width; j++) {
            
            int16_t px = x + j;
            int16_t py = y + i;

            // perfectly safe clipping: skips pixels off any of the 4 edges
            if (px < 0 || px >= screen_w || py < 0 || py >= screen_h) {
                continue;
            }

            uint32_t color = sp->map[i * sp->width + j];
            
            if (color != transparent) {
                vg_draw_pixel((uint16_t)px, (uint16_t)py, color);
            }
        }
    }
    
    return 0;
}

int draw_sprite_flipped(sprite_t *sp, int16_t x, int16_t y) {
    if (sp == NULL || sp->map == NULL) return 1;

    uint32_t transparent = xpm_transparency_color(XPM_8_8_8_8);

    int16_t screen_w = (int16_t) vg_get_x_res();
    int16_t screen_h = (int16_t) vg_get_y_res();

    for (int16_t i = 0; i < (int16_t) sp->height; i++) {
        for (int16_t j = 0; j < (int16_t) sp->width; j++) {

            int16_t px = x + j;
            int16_t py = y + i;

            if (px < 0 || px >= screen_w || py < 0 || py >= screen_h) {
                continue;
            }

            uint32_t color = sp->map[i * sp->width + (sp->width - 1 - j)];

            if (color != transparent) {
                vg_draw_pixel((uint16_t)px, (uint16_t)py, color);
            }
        }
    }

    return 0;
}
