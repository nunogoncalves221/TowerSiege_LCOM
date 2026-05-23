#include <lcom/lcf.h>
#include "video.h"

// global statics
static void *video_mem; // process address where vram is mapped
static uint8_t *double_buffer = NULL; // backstage buffer for smooth drawing
static vbe_mode_info_t vmi; // vbe mode info
static unsigned int bytes_per_pixel;
static unsigned int vram_size; // keep track of the total size for the memcpy

void* (vg_init)(uint16_t mode) {

    // get mode info
    memset(&vmi, 0, sizeof(vmi));
    if (vbe_get_mode_info(mode, &vmi) != OK) return NULL;

    bytes_per_pixel = (vmi.BitsPerPixel + 7) / 8;

    // map physical memory to the process address space
    int r;
    struct minix_mem_range mr;
    unsigned int vram_base = vmi.PhysBasePtr;
    vram_size = vmi.XResolution * vmi.YResolution * bytes_per_pixel;

    mr.mr_base = (phys_bytes) vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
        return NULL;
    }

    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
    if (video_mem == MAP_FAILED) return NULL;

    // allocate our double buffer dynamically
    double_buffer = (uint8_t *) malloc(vram_size);
    if (double_buffer == NULL) {
        printf("Failed to allocate double buffer!\n");
        return NULL;
    }
    
    // clean the buffer (make it black) so we don't draw garbage on startup
    memset(double_buffer, 0, vram_size);

    // set graphics mode
    reg86_t reg;
    memset(&reg, 0, sizeof(reg));
    reg.intno = 0x10; // bios video service
    reg.ax = 0x4F02; // vbe call to set mode
    reg.bx = BIT(14) | mode; // set bit 14 for linear frame buffer

    if (sys_int86(&reg) != OK) {
        printf("Set Value Mode failed\n");
        free(double_buffer); // don't leak memory if we fail
        return NULL;
    }

    return video_mem;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
    // check boundaries so we don't write outside our buffer and crash
    if (x >= vmi.XResolution || y >= vmi.YResolution) return 0; 
    
    // draw to our hidden double_buffer instead of video_mem
    uint8_t *ptr = double_buffer + (y * vmi.XResolution + x) * bytes_per_pixel;
    
    // copy the color into the buffer
    memcpy(ptr, &color, bytes_per_pixel);
    
    return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (uint16_t i = 0; i < len; i++) {
        if (vg_draw_pixel(x + i, y, color) != 0) return 1;
    }
    return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (uint16_t i = 0; i < height; i++) {
        if (vg_draw_hline(x, y + i, width, color) != 0) {
            return 1;
        }
    }
    return 0;
}

unsigned (get_bytes_per_pixel)() {
    return bytes_per_pixel;
}

// dumps everything we drew to the actual screen in one go
void (vg_refresh)() {
    if (double_buffer != NULL && video_mem != NULL) {
        memcpy(video_mem, double_buffer, vram_size);
    }
}

// clears the double buffer back to black for the next frame
void (vg_clear_buffer)() {
    if (double_buffer != NULL) {
        memset(double_buffer, 0, vram_size);
    }
}

// frees the memory when the game is over to avoid leaks
void (vg_free)() {
    if (double_buffer != NULL) {
        free(double_buffer);
        double_buffer = NULL;
    }
}

unsigned (vg_get_x_res)() {
    return vmi.XResolution;
}

unsigned (vg_get_y_res)() {
    return vmi.YResolution;
}

void (vg_blit_region)(const uint8_t *src, uint16_t src_stride_px, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    if (double_buffer == NULL || src == NULL) return;
    for (uint16_t row = 0; row < h; row++) {
        if (y + row >= vmi.YResolution) break;
        uint16_t clamped_w = (x + w > vmi.XResolution) ? vmi.XResolution - x : w;
        uint8_t *dst_ptr = double_buffer + ((y + row) * vmi.XResolution + x) * bytes_per_pixel;
        const uint8_t *src_ptr = src + (uint32_t)row * src_stride_px * bytes_per_pixel;
        memcpy(dst_ptr, src_ptr, clamped_w * bytes_per_pixel);
    }
}

void (vg_copy_region)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *dst, uint16_t dst_stride_px) {
    if (double_buffer == NULL || dst == NULL) return;
    for (uint16_t row = 0; row < h; row++) {
        if (y + row >= vmi.YResolution) break;
        uint16_t clamped_w = (x + w > vmi.XResolution) ? vmi.XResolution - x : w;
        const uint8_t *src_ptr = double_buffer + ((y + row) * vmi.XResolution + x) * bytes_per_pixel;
        uint8_t *dst_ptr = dst + (uint32_t)row * dst_stride_px * bytes_per_pixel;
        memcpy(dst_ptr, src_ptr, clamped_w * bytes_per_pixel);
    }
}
