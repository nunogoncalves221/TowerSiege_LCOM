#ifndef __VIDEO_H
#define __VIDEO_H

#include <lcom/lcf.h>
#include <stdint.h>

//video card functions
void* (vg_init)(uint16_t mode);
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
unsigned (get_bytes_per_pixel)();
unsigned (vg_get_x_res)();
unsigned (vg_get_y_res)();
void (vg_refresh)();
void (vg_clear_buffer)();
void (vg_free)();
void (vg_blit_region)(const uint8_t *src, uint16_t src_stride_px, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void (vg_copy_region)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *dst, uint16_t dst_stride_px);

#endif
