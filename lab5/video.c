#include <lcom/lcf.h>
#include "video.h"

// global statics
static void *video_mem; // process address where vram is mapped
static vbe_mode_info_t vmi; // vbe mode info

void* (vg_init)(uint16_t mode) {

    // get mode info
    memset(&vmi, 0, sizeof(vmi));
    if (vbe_get_mode_info(mode, &vmi) != OK) return NULL;

    // map physical memory to the process address space
    int r;
    struct minix_mem_range mr;
    unsigned int vram_base = vmi.PhysBasePtr;
    unsigned int vram_size = vmi.XResolution * vmi.YResolution * ((vmi.BitsPerPixel + 7) / 8);

    mr.mr_base = (phys_bytes) vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
        return NULL;
    }

    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
    if (video_mem == MAP_FAILED) return NULL;

    // set graphics mode
    reg86_t reg;
    memset(&reg, 0, sizeof(reg));
    reg.intno = 0x10; // bios video service
    reg.ax = 0x4F02; // vbe call to set mode
    reg.bx = BIT(14) | mode; // set bit 14 for linear frame buffer

    if (sys_int86(&reg) != OK) {
        printf("Set Value Mode failed\n");
        return NULL;
    }

    return video_mem;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) { return 0; }
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) { return 0; }
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) { return 0; }
