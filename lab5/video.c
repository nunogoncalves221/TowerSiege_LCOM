#include <lcom/lcf.h>
#include <stdint.h>
#include <string.h>

#include "video.h"

static char *video_mem;
static vbe_mode_info_t vmi;
static unsigned bytes_per_pixel;

void *(vg_init)(uint16_t mode) {
    struct minix_mem_range mr;
    unsigned int vram_size;

    if (vbe_get_mode_info(mode, &vmi) != OK)
        return NULL;

    bytes_per_pixel = (vmi.BitsPerPixel + 7) / 8;
    vram_size = vmi.XResolution * vmi.YResolution * bytes_per_pixel;

    mr.mr_base = (phys_bytes) vmi.PhysBasePtr;
    mr.mr_limit = mr.mr_base + vram_size;

    if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != OK)
        return NULL;

    video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

    if (video_mem == MAP_FAILED)
        return NULL;

    reg86_t reg;
    memset(&reg, 0, sizeof(reg));
    reg.intno = 0x10;
    reg.ax = 0x4F02;
    reg.bx = mode | BIT(14);

    if (sys_int86(&reg) != OK)
        return NULL;

    return video_mem;
}

int(vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
    if (x >= vmi.XResolution || y >= vmi.YResolution)
        return 1;

    unsigned int offset = (y * vmi.XResolution + x) * bytes_per_pixel;

    memcpy(video_mem + offset, &color, bytes_per_pixel);

    return 0;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (uint16_t i = 0; i < len; i++) {
        if (vg_draw_pixel(x + i, y, color) != 0)
            return 1;
    }

    return 0;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (uint16_t i = 0; i < height; i++) {
        if (vg_draw_hline(x, y + i, width, color) != 0)
            return 1;
    }

    return 0;
}
